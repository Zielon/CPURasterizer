#pragma once

#include "Vec.h"

namespace Math
{
	template <class T>
	class Vec<2, T>
	{
	public:
		T x{}, y{};

		__forceinline Vec() = default;
		__forceinline Vec(const Vec& vec): x(vec.x), y(vec.y) {}
		__forceinline Vec(const T& vec) : x(vec), y(vec) {}
		__forceinline Vec(const T& tx, const T& ty) : x(tx), y(ty) {}
		__forceinline Vec(const glm::vec<2, T>& vec) : x(vec.x), y(vec.y) {}

		__forceinline Vec& operator =(const Vec& vec)
		{
			x = vec.x;
			y = vec.y;
			return *this;
		}

		~Vec() = default;

		[[nodiscard]] __forceinline T Sum() const { return x + y; }
		[[nodiscard]] __forceinline T Product() const { return x * y; }

		__forceinline const T& operator [](const size_t idx) const { return (&x)[idx]; }
		__forceinline T& operator [](const size_t idx) { return (&x)[idx]; }

		__forceinline Vec operator +() const { return Vec(+x, +y); }
		__forceinline Vec operator -() const { return Vec(-x, -y); }
		__forceinline Vec operator +(const Vec& rhs) const { return Vec(x + rhs.x, y + rhs.y); }
		__forceinline Vec operator -(const Vec& rhs) const { return Vec(x - rhs.x, y - rhs.y); }
		__forceinline Vec operator *(const Vec& rhs) const { return Vec(x * rhs.x, y * rhs.y); }
		__forceinline Vec operator *(const T& rhs) const { return Vec(x * rhs, y * rhs); }
		__forceinline Vec operator /(const Vec& rhs) const { return Vec(x / rhs.x, y / rhs.y); }
		__forceinline Vec operator /(const T& rhs) const { return Vec(x / rhs, y / rhs); }
	};
}
