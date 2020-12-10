#include "Renderer.h"

#include <array>

#include "Scene.h"
#include "Camera.h"
#include "Concurrency.h"
#include "Clipper.h"
#include "Triangle.h"

namespace Engine
{
	Renderer::Renderer(const Scene& scene, const Camera& camera) : scene(scene), camera(camera)
	{
		CreateBuffers();
		CreateTiles();

		colorBuffer.reset(new ColorBuffer());
		depthBuffer.reset(new DepthBuffer());
		fragmentShader.reset(new PhongBlinnShader());
		vertexShader.reset(new DefaultVertexShader(camera));
		rasterizer.reset(new LarrabeeRasterizer(std::ref(rasterTrianglesBuffer), std::ref(tiles)));
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
				auto& v1 = projectedVertexStorage[scene.GetIndexBuffer()[3 * i + 1]];
				auto& v2 = projectedVertexStorage[scene.GetIndexBuffer()[3 * i + 2]];

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
	}

	void Renderer::FragmentShaderStage()
	{
		Concurrency::ForEach(coreIds.begin(), coreIds.end(), [this](int bin) { });
	}

	void Renderer::UpdateFrameBuffer() { }

	void Renderer::UpdateState(const Settings& settings)
	{
		this->settings = settings;
	}

	void Renderer::CreateTiles()
	{
		int id = 0;

		tiles.resize(HEIGHT / TILE_SIZE * WIDTH / TILE_SIZE);

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

		coreIds.resize(numCores);
		std::iota(coreIds.begin(), coreIds.end(), 0);
	}
}
