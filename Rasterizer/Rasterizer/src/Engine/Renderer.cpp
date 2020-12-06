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
		vertexShader.reset(new BasicVertexShader());
		fragmentShader.reset(new PhongBlinnShader());
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
		std::fill(framebuffer.begin(), framebuffer.end(), Assets::Color4b(50, 50, 50));
	}

	void Renderer::RunVertexShader() {}
	void Renderer::RunClipping() {}
	void Renderer::TiledRasterization() {}
	void Renderer::RunFragmentShader() {}
	void Renderer::UpdateFrameBuffer() {}
	void Renderer::UpdateState(const Settings& settings) {}
}
