#include "Renderer.h"

#include <array>

#include "Scene.h"
#include "Camera.h"
#include "Concurrency.h"
#include "Clipper.h"
#include "Triangle.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include "../SIMD/AVX.h"

#include "../Math/Math.h"

namespace Engine
{
	Renderer::Renderer(const Scene& scene, const Camera& camera) : scene(scene), camera(camera)
	{
		CreateTiles();
		CreateBuffers();

		colorBuffer.reset(new ColorBuffer());
		depthBuffer.reset(new DepthBuffer(settings));
		fragmentShader.reset(new PhongBlinnShader());
		vertexShader.reset(new DefaultVertexShader(camera));
		rasterizer.reset(new LarrabeeRasterizer(
			rasterTrianglesBuffer, tiles, clippedProjectedVertexBuffer, *depthBuffer));
	}

	void Renderer::Render(const Settings& settings)
	{
		UpdateState(settings);

		// Full rendering pass
		Clear();

		VertexShaderStage();
		ClippingStage();
		TiledRasterizationStage();
		RasterizationStage();
		FragmentShaderStage();

		UpdateFrameBuffer();
	}

	const uint8_t* Renderer::GetColorBuffer() const
	{
		return colorBuffer->Get();
	}

	void Renderer::Clear()
	{
		colorBuffer->Clear();
		depthBuffer->Clear();

		Concurrency::ForEach(tiles.begin(), tiles.end(), [this](Tile& tile) { tile.Clear(); });
		Concurrency::ForEach(coreIds.begin(), coreIds.end(), [this](int bin)
		{
			clippedProjectedVertexBuffer[bin].clear();
			rasterTrianglesBuffer[bin].clear();
		});
	}

	void Renderer::VertexShaderStage()
	{
		const auto& buffer = scene.GetVertexBuffer();

		Concurrency::ForEach(buffer.begin(), buffer.end(), [this](const Assets::Vertex& inVertex)
		{
			vertexShader->Process(inVertex, projectedVertexStorage[inVertex.id]);
		});
	}

	void Renderer::ClippingStage()
	{
		uint32_t size = scene.GetTriangleCount();
		uint32_t interval = (size + numCores - 1) / numCores;

		Concurrency::ForEach(coreIds.begin(), coreIds.end(), [this, interval, size](int bin)
		{
			// Assign each thread to separate chunk of buffer
			const auto startIdx = bin * interval;
			const auto endIdx = (bin + 1) * interval;

			auto& clippedBuffer = clippedProjectedVertexBuffer[bin];

			for (uint32_t i = startIdx; i < endIdx; ++i)
			{
				if (3 * i + 2 > scene.GetIndexBuffer().size())
					break;

				auto& v0 = projectedVertexStorage[scene.GetIndexBuffer()[3 * i + 0]];
				auto& v1 = projectedVertexStorage[scene.GetIndexBuffer()[3 * i + 2]];
				auto& v2 = projectedVertexStorage[scene.GetIndexBuffer()[3 * i + 1]];

				uint32_t clipCode0 = Clipper::ClipCode(v0.projectedPosition);
				uint32_t clipCode1 = Clipper::ClipCode(v1.projectedPosition);
				uint32_t clipCode2 = Clipper::ClipCode(v2.projectedPosition);

				if (clipCode0 | clipCode1 | clipCode2) continue;

				uint32_t idx0 = clippedBuffer.size();
				clippedBuffer.push_back(v0);
				uint32_t idx1 = clippedBuffer.size();
				clippedBuffer.push_back(v1);
				uint32_t idx2 = clippedBuffer.size();
				clippedBuffer.push_back(v2);

				// Perspective division
				float inv = 1.f / v0.projectedPosition.w;
				glm::vec4 hv0 = v0.projectedPosition * inv;

				inv = 1.f / v1.projectedPosition.w;
				glm::vec4 hv1 = v1.projectedPosition * inv;

				inv = 1.f / v2.projectedPosition.w;
				glm::vec4 hv2 = v2.projectedPosition * inv;

				auto raster = camera.GetRasterMatrix();

				glm::vec4 r0 = raster * hv0;
				glm::vec4 r1 = raster * hv1;
				glm::vec4 r2 = raster * hv2;

				auto triId = rasterTrianglesBuffer[bin].size();

				LarrabeeTriangle triangle(r0, r1, r2, triId, bin, 0, { idx0, idx1, idx2 });

				if (triangle.Setup())
				{
					rasterTrianglesBuffer[bin].push_back(triangle);
				}
			}
		});

		Concurrency::ForEach(coreIds.begin(), coreIds.end(), [this](int bin)
		{
			for (auto i = 0; i < clippedProjectedVertexBuffer[bin].size(); i++)
			{
				Assets::Vertex& vertex = clippedProjectedVertexBuffer[bin][i];
				vertex.invW = 1.0f / vertex.projectedPosition.w;
				vertex.projectedPosition.z *= vertex.invW;
			}
		});
	}

	void Renderer::TiledRasterizationStage()
	{
		Concurrency::ForEach(coreIds.begin(), coreIds.end(), [this](int bin)
		{
			rasterizer->AssignTriangles(bin);
		});
	}

	void Renderer::RasterizationStage()
	{
		Concurrency::ForEach(tiles.begin(), tiles.end(), [this](Tile& tile)
		{
			for (auto bin : coreIds)
				rasterizer->RasterizeTile(bin, tile);
		});

		pixels.clear();
		tiledPixels.clear();

		tiledPixels.resize(tiles.size());
		for (auto i = 0; i < tiles.size(); i++)
		{
			tiledPixels[i].resize(tiles[i].fragments.size());
			auto it = pixels.end();
			if (!tiles[i].fragments.empty())
				pixels.insert(it, tiles[i].fragments.begin(), tiles[i].fragments.end());
		}
	}

	const __m256 absmask = _mm256_castsi256_ps(_mm256_set1_epi32(~(1 << 31)));

	__m256 vecabs_and(__m256 v)
	{
		return _mm256_and_ps(absmask, v);
	}

	AVXVec3f absVec(AVXVec3f& vec)
	{
		AVXVec3f out;
		out.x = vecabs_and(vec.x);
		out.y = vecabs_and(vec.y);
		out.z = vecabs_and(vec.z);

		return out;
	}

	void Renderer::FragmentShaderStage()
	{
		Concurrency::ForEach(pixels.begin(), pixels.end(), [&](Pixel& frag)
		{
			const Assets::Vertex& v0 = clippedProjectedVertexBuffer[frag.coreId][frag.vId0];
			const Assets::Vertex& v1 = clippedProjectedVertexBuffer[frag.coreId][frag.vId1];
			const Assets::Vertex& v2 = clippedProjectedVertexBuffer[frag.coreId][frag.vId2];

			AVXVec3f position;
			AVXVec3f normal;
			AVXVec2f texCoord;

			frag.Interpolate(v0, v1, v2, frag.lambda0, frag.lambda1, position, normal, texCoord);

			auto lightDir = glm::vec3(1, 1, -1);

			AVXFloat w = Math::Rsqrt(Dot(normal, normal));
			AVXVec3f _normal = normal * w;
			AVXVec3f vecLightDir = AVXVec3f(lightDir);

			//AVXFloat diffuseAmount = Dot(vecLightDir, _normal);
			//AVXBool mask = diffuseAmount < AVXFloat(0);
			//diffuseAmount = AVX::Select(mask, AVXFloat(0), diffuseAmount);
			//AVXFloat diffuse = (diffuseAmount + 0.2f) * 3 * M_PI;

			AVXVec3f shadingResults = absVec(_normal);

			Assets::Color4b colorByte[8];
			colorByte[0].FromFloats(shadingResults.x[0], shadingResults.y[0], shadingResults.z[0]);
			colorByte[1].FromFloats(shadingResults.x[1], shadingResults.y[1], shadingResults.z[1]);
			colorByte[2].FromFloats(shadingResults.x[2], shadingResults.y[2], shadingResults.z[2]);
			colorByte[3].FromFloats(shadingResults.x[3], shadingResults.y[3], shadingResults.z[3]);
			colorByte[4].FromFloats(shadingResults.x[4], shadingResults.y[4], shadingResults.z[4]);
			colorByte[5].FromFloats(shadingResults.x[5], shadingResults.y[5], shadingResults.z[5]);
			colorByte[6].FromFloats(shadingResults.x[6], shadingResults.y[6], shadingResults.z[6]);
			colorByte[7].FromFloats(shadingResults.x[7], shadingResults.y[7], shadingResults.z[7]);

			tiledPixels[frag.tileId][frag.intraTileIdx] = _mm256_loadu_si256((__m256i*)&colorByte);
		});
	}

	void Renderer::UpdateFrameBuffer()
	{
		Concurrency::ForEach(0, tiledPixels.size(), [&](int i)
		{
			for (auto j = 0; j < tiles[i].fragments.size(); j++)
			{
				const Pixel& frag = tiles[i].fragments[j];

				int maskShift = 0;
				const AVXInt& quadResults = tiledPixels[i][j];
				
				if (frag.coverageMask.GetBit(maskShift) != 0)
				{
					colorBuffer->SetColor(Assets::Color4b(quadResults.m256.m256i_u8[0],
					                                      quadResults.m256.m256i_u8[1],
					                                      quadResults.m256.m256i_u8[2]),
					                      frag.x, frag.y);
				}
				if (frag.coverageMask.GetBit(maskShift + 1) != 0)
				{
					colorBuffer->SetColor(Assets::Color4b(quadResults.m256.m256i_u8[4],
					                                      quadResults.m256.m256i_u8[5],
					                                      quadResults.m256.m256i_u8[6]),
					                      frag.x + 1, frag.y);
				}
				if (frag.coverageMask.GetBit(maskShift + 2) != 0)
				{
					colorBuffer->SetColor(Assets::Color4b(quadResults.m256.m256i_u8[8],
					                                      quadResults.m256.m256i_u8[9],
					                                      quadResults.m256.m256i_u8[10]),
					                      frag.x, frag.y + 1);
				}
				if (frag.coverageMask.GetBit(maskShift + 3) != 0)
				{
					colorBuffer->SetColor(Assets::Color4b(quadResults.m256.m256i_u8[12],
					                                      quadResults.m256.m256i_u8[13],
					                                      quadResults.m256.m256i_u8[14]),
					                      frag.x + 1, frag.y + 1);
				}

				// ===========================
				
				if (frag.coverageMask.GetBit(maskShift + 4) != 0)
				{
					colorBuffer->SetColor(Assets::Color4b(quadResults.m256.m256i_u8[16],
					                                      quadResults.m256.m256i_u8[17],
					                                      quadResults.m256.m256i_u8[18]),
					                      frag.x + 2, frag.y);
				}
				if (frag.coverageMask.GetBit(maskShift + 5) != 0)
				{
					colorBuffer->SetColor(Assets::Color4b(quadResults.m256.m256i_u8[20],
					                                      quadResults.m256.m256i_u8[21],
					                                      quadResults.m256.m256i_u8[22]),
					                      frag.x + 3, frag.y);
				}
				if (frag.coverageMask.GetBit(maskShift + 6) != 0)
				{
					colorBuffer->SetColor(Assets::Color4b(quadResults.m256.m256i_u8[24],
					                                      quadResults.m256.m256i_u8[25],
					                                      quadResults.m256.m256i_u8[26]),
					                      frag.x + 2, frag.y + 1);
				}
				if (frag.coverageMask.GetBit(maskShift + 7) != 0)
				{
					colorBuffer->SetColor(Assets::Color4b(quadResults.m256.m256i_u8[28],
					                                      quadResults.m256.m256i_u8[29],
					                                      quadResults.m256.m256i_u8[30]),
					                      frag.x + 3, frag.y + 1);
				}
			}
		});
	}

	void Renderer::UpdateState(const Settings& settings)
	{
		this->settings = settings;
	}

	void Renderer::CreateTiles()
	{
		int id = 0;

		tiles.resize(TILE_DIM_X * TILE_DIM_Y);

		for (int i = 0; i < HEIGHT; i += TILE_SIZE)
			for (int j = 0; j < WIDTH; j += TILE_SIZE)
			{
				const auto maxX = std::min(j + TILE_SIZE, WIDTH);
				const auto maxY = std::min(i + TILE_SIZE, HEIGHT);
				tiles[id] = Tile(glm::ivec2(j, i), glm::ivec2(maxX, maxY), id);
				++id;
			}
	}

	void Renderer::CreateBuffers()
	{
		numCores = std::thread::hardware_concurrency();

		projectedVertexStorage.resize(scene.GetVertexBuffer().size());
		rasterTrianglesBuffer.resize(numCores);
		clippedProjectedVertexBuffer.resize(numCores);

		tiledPixels.reserve(tiles.size());
		pixels.reserve(tiles.size() * TILE_SIZE * TILE_SIZE / 4);

		coreIds.resize(numCores);
		std::iota(coreIds.begin(), coreIds.end(), 0);
	}
}
