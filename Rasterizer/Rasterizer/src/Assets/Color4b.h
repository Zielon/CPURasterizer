#pragma once

#include <algorithm>
#include <cstdint>

#define GLM_FORCE_INLINE 
#include <glm/glm.hpp>

#include "../Config.h"

namespace Assets
{
	class Color4b
	{
	public:
		uint8_t r{}, g{}, b{}, a{};

		Color4b() = default;
		Color4b(uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255): r(R), g(G), b(B), a(A) {}

		CFG_FORCE_INLINE Color4b operator *(float val) const
		{
			return Color4b(uint8_t(val * r), uint8_t(val * g), uint8_t(val * b));
		}

		CFG_FORCE_INLINE Color4b operator +(const Color4b& color) const
		{
			return Color4b(r + color.r, g + color.g, b + color.b);
		}

		CFG_FORCE_INLINE Color4b& operator +=(const Color4b& color)
		{
			r += color.r;
			g += color.g;
			b += color.b;
			a = 1;

			return *this;
		}

		CFG_FORCE_INLINE Color4b operator /(float val) const
		{
			float fInv = 1.0f / val;
			return Color4b(uint8_t(r * fInv), uint8_t(g * fInv), uint8_t(b * fInv));
		}

		[[nodiscard]] CFG_FORCE_INLINE glm::vec3 ToneMap(const glm::vec3& rgb, float limit) const
		{
			const float luminance = 0.299f * rgb.x + 0.587f * rgb.y + 0.114f * rgb.z;
			return rgb / (1.0f + luminance / limit);
		}

		[[nodiscard]] CFG_FORCE_INLINE glm::vec3 GammaCorrection(const glm::vec3& ldr) const
		{
			constexpr auto exponent = glm::vec3(1.0f / 2.2f);
			return pow(ldr, exponent);
		}

		CFG_FORCE_INLINE void LDR(float R, float G, float B, bool gammaCorrection, float A = 1.0f)
		{
			if (gammaCorrection)
			{
				const auto ldr = GammaCorrection({ R, G, B }) * 255.f;
				const auto rgb = clamp(ldr, 0.f, 255.f);

				r = uint8_t(rgb.x);
				g = uint8_t(rgb.y);
				b = uint8_t(rgb.z);
				a = 1;
			}
			else
			{
				r = uint8_t(std::clamp(255.f * R, 0.f, 255.f));
				g = uint8_t(std::clamp(255.f * G, 0.f, 255.f));
				b = uint8_t(std::clamp(255.f * B, 0.f, 255.f));
				a = uint8_t(std::clamp(255.f * A, 0.f, 255.f));
			}
		}
	};
}
