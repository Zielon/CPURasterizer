#pragma once

#include <array>
#include <memory>
#include <thread>

#include "Settings.h"
#include "Tile.h"

#include "../Assets/Color4b.h"
#include "../Assets/Vertex.h"

#include "Shaders/FragmentShader.h"

using FrameBuffer = std::array<Assets::Color4b, WIDTH * HEIGHT>;
using VertexBuffer = std::vector<Assets::Vertex>;

namespace Engine
{
	class Renderer final
	{
	public:
		Renderer(const class Scene& scene, const class Camera& camera);

		void Render(const Settings& settings);

		[[nodiscard]] const uint8_t* GetFrameBuffer() const;

	private:
		void Clear();
		void RunVertexShader();
		void RunClipping();
		void TiledRasterization();
		void RunFragmentShader();
		void UpdateFrameBuffer();
		void UpdateState(const Settings& settings);
		void CreateTiles();

		const Scene& scene;
		const Camera& camera;

		Settings settings{};
		FrameBuffer framebuffer;
		VertexBuffer projectedVertexBuffer;

		std::vector<uint32_t> coreIds;
		std::vector<Tile> tiles;
		std::unique_ptr<FragmentShader> fragmentShader;
	};
}
