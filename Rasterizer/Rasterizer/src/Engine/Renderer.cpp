#include "Renderer.h"

#include <array>

#include "Scene.h"
#include "Camera.h"
#include "Concurrency.h"
#include "Clipper.h"
#include "Larrabee.h"
#include "Triangle.h"

#include "../Assets/Color4b.h"

namespace Engine
{
	Renderer::Renderer(const Scene& scene, const Camera& camera) : scene(scene), camera(camera)
	{
		fragmentShader.reset(new PhongBlinnShader());

		CreateBuffers();
		CreateTiles();
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

	const uint8_t* Renderer::GetFrameBuffer() const
	{
		return reinterpret_cast<const uint8_t*>(frameBuffer.data());
	}

	void Renderer::Clear()
	{
		Concurrency::Fill(frameBuffer.begin(), frameBuffer.end(), Assets::Color4b(0, 0, 0));
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

		Concurrency::ForEach(
			buffer.begin(), buffer.end(),
			[this](const Assets::Vertex& inVertex)
			{
				auto& outVertex = projectedVertexStorage[inVertex.id];

				outVertex.projectedPosition =
					camera.GetProjectionMatrix() * camera.GetViewMatrix() * glm::vec4(inVertex.position, 1.f);
				outVertex.position = inVertex.position;
				outVertex.normal = inVertex.normal;
				outVertex.texCoords = inVertex.texCoords;
				outVertex.id = inVertex.id;
			});
	}

	void Renderer::ClippingStage()
	{
		uint32_t size = scene.GetIndexBuffer().size();
		uint32_t interval = (size + numCores - 1) / numCores;

		Concurrency::ForEach(coreIds.begin(), coreIds.end(), [this, interval, size](int bin)
		{
			// Assign each thread to separate chunk of buffer
			const auto startIdx = bin * interval;
			const auto endIdx = (bin + 1) * interval;

			auto& clippedBuffer = clippedProjectedVertexBuffer[bin];

			for (uint32_t i = startIdx; i < endIdx; i += 3)
			{
				if (i + 3 >= size)
					return;

				auto& v0 = projectedVertexStorage[scene.GetIndexBuffer()[i + 0]];
				auto& v1 = projectedVertexStorage[scene.GetIndexBuffer()[i + 1]];
				auto& v2 = projectedVertexStorage[scene.GetIndexBuffer()[i + 2]];

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
			Larrabee::AssignTriangles(bin, rasterTrianglesBuffer[bin], tiles);
		});
	}

	void Renderer::RasterizationStage()
	{
		Concurrency::ForEach(tiles.begin(), tiles.end(), [this](Tile& tile)
		{
			for (auto bin : coreIds)
			{
				for (int i = 0; i < tile.binsIndex[bin]; ++i)
				{
					const uint32_t id = tile.trinagles[bin][i];
					LarrabeeTriangle& triangle = rasterTrianglesBuffer[bin][id];

					for (auto i = tile.minRaster.y; i < tile.maxRaster.y; ++i)
						for (auto j = tile.minRaster.x; j < tile.maxRaster.x; ++j)
						{
							frameBuffer[i * WIDTH + j] = tile.color;
						}
				}
			}
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

		for (int i = 0; i < HEIGHT; i += TILE_SIZE)
			for (int j = 0; j < WIDTH; j += TILE_SIZE)
			{
				const auto maxX = std::min(j + TILE_SIZE, WIDTH);
				const auto maxY = std::min(i + TILE_SIZE, HEIGHT);
				tiles.emplace_back(glm::ivec2(j, i), glm::ivec2(maxX, maxY), id);
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
