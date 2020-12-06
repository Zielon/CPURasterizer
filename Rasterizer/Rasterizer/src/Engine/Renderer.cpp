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
		std::fill(framebuffer.begin(), framebuffer.end(), Assets::Color4b(0, 0, 0));
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
			});
	}

	void Renderer::RunClipping() {}
	void Renderer::TiledRasterization() {}
	void Renderer::RunFragmentShader() {}
	void Renderer::UpdateFrameBuffer() {}
	void Renderer::UpdateState(const Settings& settings) {}
}
