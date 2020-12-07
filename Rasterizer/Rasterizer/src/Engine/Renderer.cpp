#include "Renderer.h"

#include <array>

#include "Scene.h"
#include "Camera.h"
#include "Concurrency.h"
#include "Clipper.h"

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
					camera.GetProjection() * camera.GetView() * glm::vec4(inVertex.position, 1.f);
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

				int idx0 = clippedBuffer.size();
				clippedBuffer.push_back(v0);
				int idx1 = clippedBuffer.size();
				clippedBuffer.push_back(v1);
				int idx2 = clippedBuffer.size();
				clippedBuffer.push_back(v2);
			}
		});
	}

	void Renderer::TiledRasterizationStage()
	{
		Concurrency::ForEach(coreIds.begin(), coreIds.end(), [this](int bin) { });
	}

	void Renderer::FragmentShaderStage() {}

	void Renderer::UpdateFrameBuffer() {}

	void Renderer::UpdateState(const Settings& settings)
	{
		this->settings = settings;
	}

	void Renderer::CreateTiles()
	{
		tiles.resize(HEIGHT / TILE * WIDTH / TILE);

		int id = 0;

		for (int i = 0; i < HEIGHT; i += TILE)
		{
			for (int j = 0; j < WIDTH; j += TILE)
			{
				const auto maxX = std::min(j + TILE, WIDTH);
				const auto maxY = std::min(i + TILE, HEIGHT);

				tiles[id] = Tile(glm::ivec2(j, i), glm::ivec2(maxX, maxY), id);

				++id;
			}
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
