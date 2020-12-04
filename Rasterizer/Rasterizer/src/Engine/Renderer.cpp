#include "Renderer.h"

#include <array>

#include "../Assets/Color4b.h"

namespace Engine
{
	Renderer::Renderer() { }

	void Renderer::Render(const Settings& settings)
	{
		this->settings = settings;

		std::fill(framebufferArray.begin(), framebufferArray.end(), Assets::Color4b(50, 50, 50));
	}

	const uint8_t* Renderer::GetFrameBuffer() const
	{
		return reinterpret_cast<const uint8_t*>(framebufferArray.data());
	}
}
