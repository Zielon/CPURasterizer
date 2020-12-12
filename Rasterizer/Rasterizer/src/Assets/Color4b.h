#pragma once

#include <algorithm>
#include <cstdint>

namespace Assets
{
	class Color4b
	{
	public:
		uint8_t r{}, g{}, b{}, a{};

		Color4b() = default;
		Color4b(uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255): r(R), g(G), b(B), a(A) {}

		__forceinline Color4b operator *(float val) const
		{
			return Color4b(uint8_t(val * r), uint8_t(val * g), uint8_t(val * b));
		}

		__forceinline Color4b operator +(const Color4b& color) const
		{
			return Color4b(r + color.r, g + color.g, b + color.b);
		}

		__forceinline Color4b& operator +=(const Color4b& color)
		{
			r += color.r;
			g += color.g;
			b += color.b;
			a = 1;

			return *this;
		}

		__forceinline Color4b operator /(float val) const
		{
			float fInv = 1.0f / val;
			return Color4b(uint8_t(r * fInv), uint8_t(g * fInv), uint8_t(b * fInv));
		}

		__forceinline Color4b operator >>(const int shift) const
		{
			return Color4b(r >> shift, g >> shift, b >> shift, a);
		}

		__forceinline void FromFloats(float R, float G, float B, float A = 1.0f)
		{
			r = uint8_t(pow(std::clamp(255.f * R, 0.f, 255.f), 1.f));
			g = uint8_t(pow(std::clamp(255.f * G, 0.f, 255.f), 1.f));
			b = uint8_t(pow(std::clamp(255.f * B, 0.f, 255.f), 1.f));
			a = uint8_t(pow(std::clamp(255.f * A, 0.f, 255.f), 1.f));
		}
	};
}
