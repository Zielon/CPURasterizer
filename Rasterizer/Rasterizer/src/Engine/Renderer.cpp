#include "Renderer.h"

#include <array>

#include "Scene.h"
#include "Camera.h"
#include "Concurrency.h"
#include "Clipper.h"
#include "Triangle.h"

#include <glm/gtx/component_wise.hpp>

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

				Triangle triangle(r0, r1, r2, bin, { idx0, idx1, idx2 });

				if (triangle.IsValid())
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
			for (int i = 0; i < rasterTrianglesBuffer[bin].size(); ++i)
			{
				const Triangle& triangle = rasterTrianglesBuffer[bin][i];

				int minX = std::max(0, std::min(triangle.v0.x, std::min(triangle.v1.x, triangle.v2.x)));
				int maxX = std::min(WIDTH - 1, std::max(triangle.v0.x, std::max(triangle.v1.x, triangle.v2.x)));
				int minY = std::max(0, std::min(triangle.v0.y, std::min(triangle.v1.y, triangle.v2.y)));
				int maxY = std::min(HEIGHT - 1, std::max(triangle.v0.y, std::max(triangle.v1.y, triangle.v2.y)));

				for (auto x = minX; x <= maxX; x++)
				{
					for (auto y = minY; y <= maxY; y++)
					{
						glm::ivec2 pixelBase(x, y);

						frameBuffer[y * WIDTH + x] = {255, 0, 0};
					}
				}
			}
		});
	}

	void Renderer::FragmentShaderStage() {}

	void Renderer::UpdateFrameBuffer()
	{
		//Concurrency::ForEach(tiles.begin(), tiles.end(), [this](Tile& tile)
		//{
		//	for (int i = tile.minRaster.x; i < tile.maxRaster.x; ++i)
		//	{
		//		for (int j = tile.minRaster.y; j < tile.maxRaster.y; ++j)
		//		{
		//			frameBuffer[i * WIDTH + j] = tile.color;
		//		}
		//	}
		//});

		/*		for (int i = 0; i < HEIGHT; ++i)
				{
					for (int j = 0; j < WIDTH; ++j)
					{
						int x = i / TILE;
						int y = j / TILE;
		
						Tile& tile = tiles[x * TILE + y];
		
						frameBuffer[i * WIDTH + j] = tile.color;
					}
				}*/
	}

	void Renderer::UpdateState(const Settings& settings)
	{
		this->settings = settings;
	}

	void Renderer::CreateTiles()
	{
		int id = 0;

		for (int i = 0; i < HEIGHT; i += TILE)
			for (int j = 0; j < WIDTH; j += TILE)
			{
				const auto maxX = std::min(j + TILE, WIDTH);
				const auto maxY = std::min(i + TILE, HEIGHT);
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
