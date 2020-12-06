#include "Renderer.h"

#include <array>
#include <execution>

#include "Scene.h"
#include "Camera.h"

#include "../Assets/Color4b.h"

namespace Engine
{
	Renderer::Renderer(const Scene& scene, const Camera& camera) : scene(scene), camera(camera)
	{
		fragmentShader.reset(new PhongBlinnShader());
		projectedVertexBuffer.resize(scene.GetVertexBuffer().size());

		coreIds.resize(std::thread::hardware_concurrency());
		std::iota(coreIds.begin(), coreIds.end(), 0);
		CreateTiles();
	}

	void Renderer::Render(const Settings& settings)
	{
		UpdateState(settings);

		// Full rendering pass
		Clear();
		RunVertexShader();
		RunClipping();
		TiledRasterization();
		RunFragmentShader();
		UpdateFrameBuffer();
	}

	const uint8_t* Renderer::GetFrameBuffer() const
	{
		return reinterpret_cast<const uint8_t*>(framebuffer.data());
	}

	void Renderer::Clear()
	{
		std::fill(std::execution::par, framebuffer.begin(), framebuffer.end(), Assets::Color4b(0, 0, 0));
		std::for_each(std::execution::par, tiles.begin(), tiles.end(), [this](Tile& tile) { tile.Clear(); });
	}

	void Renderer::RunVertexShader()
	{
		const auto& buffer = scene.GetVertexBuffer();

		std::for_each(
			std::execution::par, buffer.begin(), buffer.end(),
			[this](const Assets::Vertex& inVertex)
			{
				auto& outVertex = projectedVertexBuffer[inVertex.id];

				outVertex.projectedPosition =
					camera.GetProjection() * camera.GetView() * glm::vec4(inVertex.position, 1.f);
				outVertex.position = inVertex.position;
				outVertex.normal = inVertex.normal;
				outVertex.texCoords = inVertex.texCoords;
				outVertex.id = inVertex.id;
			});
	}

	void Renderer::RunClipping() {}

	void Renderer::TiledRasterization()
	{
		std::for_each(std::execution::par, coreIds.begin(), coreIds.end(), [this](int bin)
		{
			for (auto& tile : tiles)
			{
				for (int i = 0; i < 1000; ++i)
				{
					tile.Add(bin, i);
				}
			}
		});
	}

	void Renderer::RunFragmentShader() {}

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
}
