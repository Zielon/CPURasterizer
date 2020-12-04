#pragma once

#include <array>

#include "Settings.h"

#include "../Assets/Color4b.h"

namespace Engine
{
	class Renderer final
	{
	public:
		Renderer();

		void Render(const Settings& settings);

		[[nodiscard]] const uint8_t* GetFrameBuffer() const;

	private:
		Settings settings{};
		std::array<Assets::Color4b, Width * Height> framebufferArray;
	};
}
