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
		fragmentShader.reset(new NormalsShader());
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
		Concurrency::ForEach(coreIds.begin(), coreIds.end(), [this](int bin) {
			clippedProjectedVertexBuffer[bin].clear();
			rasterTrianglesBuffer[bin].clear();
		});

		colorBuffer->Clear();
		depthBuffer->Clear();
	}

	void Renderer::VertexShaderStage()
	{
		const auto& buffer = scene.GetVertexBuffer();

		Concurrency::ForEach(buffer.begin(), buffer.end(), [this](const Assets::Vertex& inVertex) {
			vertexShader->Process(inVertex, projectedVertexStorage[inVertex.id]);
		});
	}

	void Renderer::ClippingStage()
	{
		Concurrency::ForEach(coreIds.begin(), coreIds.end(), [this](int bin) {
			clipper->Clip(bin, settings, clippedProjectedVertexBuffer[bin], rasterTrianglesBuffer[bin]);
		});
	}

	void Renderer::TiledRasterizationStage()
	{
		Concurrency::ForEach(coreIds.begin(), coreIds.end(), [this](int bin) {
			rasterizer->AssignTriangles(bin);
		});
	}

	void Renderer::RasterizationStage()
	{
		Concurrency::ForEach(tiles.begin(), tiles.end(), [this](Tile& tile) {
			for (auto bin : coreIds)
				rasterizer->RasterizeTile(bin, tile);
		});

		CopyPixels();
	}

	void Renderer::FragmentShaderStage()
	{
		Concurrency::ForEach(pixels.begin(), pixels.end(), [&](Pixel& pixel) {

			// Triangle
			Assets::Vertex& v0 = clippedProjectedVertexBuffer[pixel.coreId][pixel.vId0];
			Assets::Vertex& v1 = clippedProjectedVertexBuffer[pixel.coreId][pixel.vId1];
			Assets::Vertex& v2 = clippedProjectedVertexBuffer[pixel.coreId][pixel.vId2];

			auto shaded = fragmentShader->Shade(v0, v1, v2, pixel);

			Assets::Color4b colorByte[8];
			for (int i = 0; i < 8; i ++)
				colorByte[i].FromFloats(shaded.x[i], shaded.y[i], shaded.z[i]);

			tiledPixels[pixel.tileId][pixel.intraTileIdx] = _mm256_loadu_si256(reinterpret_cast<__m256i*>(&colorByte));
		});
	}

	void Renderer::UpdateFrameBuffer()
	{
		Concurrency::ForEach(0, tiledPixels.size(), [&](int i) {
			for (auto j = 0; j < tiles[i].fragments.size(); j++)
			{
				const Pixel& frag = tiles[i].fragments[j];

				const int maskShift = 0;

				auto& color = tiledPixels[i][j].m256.m256i_u8;

				if (frag.coverageMask.GetBit(maskShift) != 0)
				{
					colorBuffer->SetColor(Assets::Color4b(color[0], color[1], color[2]), frag.x, frag.y);
				}

				if (frag.coverageMask.GetBit(maskShift + 1) != 0)
				{
					colorBuffer->SetColor(Assets::Color4b(color[4], color[5], color[6]), frag.x + 1, frag.y);
				}

				if (frag.coverageMask.GetBit(maskShift + 2) != 0)
				{
					colorBuffer->SetColor(Assets::Color4b(color[8], color[9], color[10]), frag.x, frag.y + 1);
				}

				if (frag.coverageMask.GetBit(maskShift + 3) != 0)
				{
					colorBuffer->SetColor(Assets::Color4b(color[12], color[13], color[14]), frag.x + 1, frag.y + 1);
				}

				if (frag.coverageMask.GetBit(maskShift + 4) != 0)
				{
					colorBuffer->SetColor(Assets::Color4b(color[16], color[17], color[18]), frag.x + 2, frag.y);
				}

				if (frag.coverageMask.GetBit(maskShift + 5) != 0)
				{
					colorBuffer->SetColor(Assets::Color4b(color[20], color[21], color[22]), frag.x + 3, frag.y);
				}

				if (frag.coverageMask.GetBit(maskShift + 6) != 0)
				{
					colorBuffer->SetColor(Assets::Color4b(color[24], color[25], color[26]), frag.x + 2, frag.y + 1);
				}

				if (frag.coverageMask.GetBit(maskShift + 7) != 0)
				{
					colorBuffer->SetColor(Assets::Color4b(color[28], color[29], color[30]), frag.x + 3, frag.y + 1);
				}
			}
		});
	}

	void Renderer::UpdateState(const Settings& settings)
	{
		this->settings = settings;
	}

	void Renderer::CopyPixels()
	{
		pixels.clear();
		tiledPixels.clear();

		tiledPixels.resize(tiles.size());
		for (auto i = 0; i < tiles.size(); i++)
		{
			tiledPixels[i].resize(tiles[i].fragments.size());
			const auto it = pixels.end();
			if (!tiles[i].fragments.empty())
				pixels.insert(it, tiles[i].fragments.begin(), tiles[i].fragments.end());
		}
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
