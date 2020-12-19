#pragma once

#include <vector>

#include "Settings.h"

#include "../Assets/Color4b.h"

namespace Engine
{
	class ColorBuffer final
	{
	public:
		ColorBuffer(int width, int height): height(height), width(width)
		{
			frameBuffer.resize(width * height);
		}

		void Clear();

		void SetColor(Assets::Color4b color, uint32_t x, uint32_t y);
		[[nodiscard]] const uint8_t* Get() const;

	private:
		int height;
		int width;
		int tileDimX;
		int tileDimY;
		std::vector<Assets::Color4b> frameBuffer;
	};
}
