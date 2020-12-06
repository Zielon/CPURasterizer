#pragma once

#include <array>
#include <memory>
#include <thread>

#include "Settings.h"

#include "../Assets/Color4b.h"
#include "../Assets/Vertex.h"

#include "Shaders/FragmentShader.h"

using FrameBuffer = std::array<Assets::Color4b, Width * Height>;
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

		const Scene& scene;
		const Camera& camera;

		uint32_t coresNum = std::thread::hardware_concurrency();
		Settings settings{};
		FrameBuffer framebuffer;
		VertexBuffer projectedVertexBuffer;

		std::unique_ptr<FragmentShader> fragmentShader;
	};
}
