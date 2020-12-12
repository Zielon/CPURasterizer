#include "ColorBuffer.h"

#include "Concurrency.h"

namespace Engine
{
	void ColorBuffer::Clear()
	{
		Concurrency::Fill(frameBuffer.begin(), frameBuffer.end(), Assets::Color4b(0, 0, 0));
	}

	void ColorBuffer::SetColor(Assets::Color4b color, uint32_t x, uint32_t y)
	{
		frameBuffer[y * width + x] = color;
	}

	const uint8_t* ColorBuffer::Get() const
	{
		return reinterpret_cast<const uint8_t*>(frameBuffer.data());
	}
}
