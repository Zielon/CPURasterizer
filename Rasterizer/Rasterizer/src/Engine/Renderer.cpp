#include "Renderer.h"

#include "Scene.h"
#include "Camera.h"
#include "Concurrency.h"
#include "Clipper.h"

#include "../SIMD/AVX.h"

#include "../Math/Math.h"

namespace Engine
{
	Renderer::Renderer(const Scene& scene, const Camera& camera) :
		scene(scene), camera(camera),
		width(camera.GetWidth()),
		height(camera.GetHeight()),
		tileDimX((width + TILE_SIZE - 1) >> TILE),
		tileDimY((height + TILE_SIZE - 1) >> TILE)
	{
		CreateTiles();
		CreateBuffers();

		colorBuffer.reset(new ColorBuffer(width, height));
		depthBuffer.reset(new DepthBuffer(width, height));
		fragmentShader.reset(new PhongBlinnShader());
		vertexShader.reset(new DefaultVertexShader(camera));
		clipper.reset(new Clipper(scene, camera, projectedVertexStorage));
		rasterizer.reset(new LarrabeeRasterizer(
			width, height, rasterTrianglesBuffer, tiles, clippedProjectedVertexBuffer, *depthBuffer));
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
		Concurrency::ForEach(tiles.begin(), tiles.end(), [this](Tile& tile) { tile.Clear(); });
		Concurrency::ForEach(coreIds.begin(), coreIds.end(), [this](int bin)
		{
			clippedProjectedVertexBuffer[bin].clear();
			rasterTrianglesBuffer[bin].clear();
		});

		colorBuffer->Clear();
		depthBuffer->Clear();
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
		Concurrency::ForEach(coreIds.begin(), coreIds.end(), [this](int bin)
		{
			clipper->Clip(bin, clippedProjectedVertexBuffer[bin], rasterTrianglesBuffer[bin]);
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
			Assets::Vertex& v0 = clippedProjectedVertexBuffer[frag.coreId][frag.vId0];
			Assets::Vertex& v1 = clippedProjectedVertexBuffer[frag.coreId][frag.vId1];
			Assets::Vertex& v2 = clippedProjectedVertexBuffer[frag.coreId][frag.vId2];

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

			AVXVec3f shadingResults = absVec(_normal) * std::pow(1, 2.2);

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

		tiles.resize(tileDimX * tileDimY);

		for (int i = 0; i < height; i += TILE_SIZE)
			for (int j = 0; j < width; j += TILE_SIZE)
			{
				const auto maxX = std::min(j + TILE_SIZE, width);
				const auto maxY = std::min(i + TILE_SIZE, height);
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
		pixels.reserve(tiles.size() * TILE_SIZE * TILE_SIZE / 2);

		coreIds.resize(numCores);
		std::iota(coreIds.begin(), coreIds.end(), 0);
	}
}
