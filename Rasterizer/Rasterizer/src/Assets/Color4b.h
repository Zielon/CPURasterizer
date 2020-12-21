#pragma once

#include <algorithm>
#include <cstdint>

#define GLM_FORCE_INLINE 
#include <glm/glm.hpp>

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

		[[nodiscard]] __forceinline glm::vec3 ToneMap(const glm::vec3& rgb, float limit) const
		{
			const float luminance = 0.299f * rgb.x + 0.587f * rgb.y + 0.114f * rgb.z;
			return rgb / (1.0f + luminance / limit);
		}

		[[nodiscard]] __forceinline glm::vec3 GammaCorrection(const glm::vec3& ldr) const
		{
			constexpr auto exponent = glm::vec3(1.0f / 2.2f);
			return pow(ldr, exponent);
		}

		__forceinline void LDR(float R, float G, float B, float A = 1.0f)
		{
			const auto ldr = GammaCorrection(ToneMap({ R, G, B }, 1.5f)) * 255.f;
			const auto rgb = clamp(ldr, 0.f, 255.f);

			r = uint8_t(rgb.x);
			g = uint8_t(rgb.y);
			b = uint8_t(rgb.z);
			a = 1;
		}
	};
}
