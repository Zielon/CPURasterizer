#pragma once

#include <array>
#include <memory>

#include "Settings.h"

#include "../Assets/Color4b.h"

#include "Shaders/FragmentShader.h"
#include "Shaders/VertexShader.h"

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
		
		Settings settings{};
		std::array<Assets::Color4b, Width * Height> framebuffer;

		std::unique_ptr<VertexShader> vertexShader;
		std::unique_ptr<FragmentShader> fragmentShader;
	};
}
