#pragma once

#include <glm/glm.hpp>

#include <cstdint>

namespace Math
{
	template <uint32_t N, class T>
	class Vec
	{
	public:
		T v[N];

		Vec() { }
	};
}
