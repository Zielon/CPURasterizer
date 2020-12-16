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
		fragmentShader.reset(new NormalShader());
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

		CopyPixelsToBuffer();
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
			for (auto j = 0; j < tiles[i].pixels.size(); j++)
			{
				const Pixel& pixel = tiles[i].pixels[j];

				const int maskShift = 0;

				auto& color = tiledPixels[i][j].m256.m256i_u8;

				if (pixel.coverageMask.GetBit(maskShift + 0) != 0)
					colorBuffer->SetColor(Assets::Color4b(color[0], color[1], color[2]), pixel.x, pixel.y);

				if (pixel.coverageMask.GetBit(maskShift + 1) != 0)
					colorBuffer->SetColor(Assets::Color4b(color[4], color[5], color[6]), pixel.x + 1, pixel.y);

				if (pixel.coverageMask.GetBit(maskShift + 2) != 0)
					colorBuffer->SetColor(Assets::Color4b(color[8], color[9], color[10]), pixel.x, pixel.y + 1);

				if (pixel.coverageMask.GetBit(maskShift + 3) != 0)
					colorBuffer->SetColor(Assets::Color4b(color[12], color[13], color[14]), pixel.x + 1, pixel.y + 1);

				if (pixel.coverageMask.GetBit(maskShift + 4) != 0)
					colorBuffer->SetColor(Assets::Color4b(color[16], color[17], color[18]), pixel.x + 2, pixel.y);

				if (pixel.coverageMask.GetBit(maskShift + 5) != 0)
					colorBuffer->SetColor(Assets::Color4b(color[20], color[21], color[22]), pixel.x + 3, pixel.y);

				if (pixel.coverageMask.GetBit(maskShift + 6) != 0)
					colorBuffer->SetColor(Assets::Color4b(color[24], color[25], color[26]), pixel.x + 2, pixel.y + 1);

				if (pixel.coverageMask.GetBit(maskShift + 7) != 0)
					colorBuffer->SetColor(Assets::Color4b(color[28], color[29], color[30]), pixel.x + 3, pixel.y + 1);
			}
		});
	}

	void Renderer::UpdateState(const Settings& settings)
	{
		this->settings = settings;
	}

	void Renderer::CopyPixelsToBuffer()
	{
		pixels.clear();
		tiledPixels.clear();
		tiledPixels.resize(tiles.size());

		for (auto i = 0; i < tiles.size(); i++)
		{
			tiledPixels[i].resize(tiles[i].pixels.size());
			const auto it = pixels.end();
			if (!tiles[i].pixels.empty())
				pixels.insert(it, tiles[i].pixels.begin(), tiles[i].pixels.end());
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
