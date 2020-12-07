#pragma once

#include <cstdint>

#include <glm/glm.hpp>

namespace Engine
{
	class Clipper final
	{
	public:
		static const uint32_t INSIDE_BIT = 0;
		static const uint32_t LEFT_BIT = 1 << 0;
		static const uint32_t RIGHT_BIT = 1 << 1;
		static const uint32_t BOTTOM_BIT = 1 << 2;
		static const uint32_t TOP_BIT = 1 << 3;
		static const uint32_t FAR_BIT = 1 << 5;
		static const uint32_t NEAR_BIT = 1 << 4;

		static uint32_t ClipCode(const glm::vec4& v)
		{
			uint32_t code = INSIDE_BIT;

			if (v.x < -v.w)
				code |= LEFT_BIT;
			if (v.x > v.w)
				code |= RIGHT_BIT;
			if (v.y < -v.w)
				code |= BOTTOM_BIT;
			if (v.y > v.w)
				code |= TOP_BIT;
			if (v.z > v.w)
				code |= FAR_BIT;
			if (v.z < 0.0f)
				code |= NEAR_BIT;

			return code;
		}
	};
}
