#pragma once

#include "Vec.h"

namespace Math
{
	template <class T>
	class Vec<3, T>
	{
	public:
		T x{}, y{}, z{};

		__forceinline Vec() = default;
		__forceinline Vec(const Vec& vec): x(vec.x), y(vec.y), z(vec.z) { }
		__forceinline Vec(const T& tVal): x(tVal), y(tVal), z(tVal) { }
		__forceinline Vec(const T& tx, const T& ty, const T& tz) : x(tx), y(ty), z(tz) { }
		__forceinline Vec(const glm::vec<3, T>& vec) : x(vec.x), y(vec.y), z(vec.z) {}

		__forceinline Vec& operator =(const Vec& vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
			return *this;
		}

		~Vec() = default;

		[[nodiscard]] __forceinline T Sum() const { return x + y + z; }
		[[nodiscard]] __forceinline T Product() const { return x * y * z; }

		__forceinline const T& operator [](const size_t idx) const { return (&x)[idx]; }
		__forceinline T& operator [](const size_t idx) { return (&x)[idx]; }

		__forceinline Vec operator +(const Vec& rhs) const { return Vec(x + rhs.x, y + rhs.y, z + rhs.z); }
		__forceinline Vec operator -(const Vec& rhs) const { return Vec(x - rhs.x, y - rhs.y, z - rhs.z); }
		__forceinline Vec operator *(const Vec& rhs) const { return Vec(x * rhs.x, y * rhs.y, z * rhs.z); }
		__forceinline Vec operator *(const T& rhs) const { return Vec(x * rhs, y * rhs, z * rhs); }
		__forceinline Vec operator /(const Vec& rhs) const { return Vec(x / rhs.x, y / rhs.y, z / rhs.z); }
		__forceinline Vec operator /(const T& rhs) const { return Vec(x / rhs, y / rhs, z / rhs); }
	};
}
