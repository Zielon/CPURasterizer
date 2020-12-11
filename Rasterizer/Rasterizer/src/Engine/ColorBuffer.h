#pragma once

#include <array>

#include "Settings.h"

#include "../Assets/Color4b.h"

namespace Engine
{
	class ColorBuffer final
	{
	public:
		void Clear();

		void SetColor(Assets::Color4b color, uint32_t x, uint32_t y);
		[[nodiscard]] const uint8_t* Get() const;

	private:
		std::array<Assets::Color4b, WIDTH * HEIGHT> frameBuffer;
	};
}
