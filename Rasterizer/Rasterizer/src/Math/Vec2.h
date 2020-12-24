#pragma once

#include "Vec.h"

#include "../Config.h"

namespace Math
{
	template <class T>
	class Vec<2, T>
	{
	public:
		T x{}, y{};

		CFG_FORCE_INLINE Vec() = default;
		CFG_FORCE_INLINE Vec(const Vec& vec): x(vec.x), y(vec.y) {}
		CFG_FORCE_INLINE Vec(const T& vec) : x(vec), y(vec) {}
		CFG_FORCE_INLINE Vec(const T& tx, const T& ty) : x(tx), y(ty) {}
		CFG_FORCE_INLINE Vec(const glm::vec<2, T>& vec) : x(vec.x), y(vec.y) {}

		CFG_FORCE_INLINE Vec& operator =(const Vec& vec)
		{
			x = vec.x;
			y = vec.y;
			return *this;
		}

		~Vec() = default;

		[[nodiscard]] CFG_FORCE_INLINE T Sum() const { return x + y; }
		[[nodiscard]] CFG_FORCE_INLINE T Product() const { return x * y; }

		CFG_FORCE_INLINE const T& operator [](const size_t idx) const { return (&x)[idx]; }
		CFG_FORCE_INLINE T& operator [](const size_t idx) { return (&x)[idx]; }

		CFG_FORCE_INLINE Vec operator +() const { return Vec(+x, +y); }
		CFG_FORCE_INLINE Vec operator -() const { return Vec(-x, -y); }
		CFG_FORCE_INLINE Vec operator +(const Vec& rhs) const { return Vec(x + rhs.x, y + rhs.y); }
		CFG_FORCE_INLINE Vec operator -(const Vec& rhs) const { return Vec(x - rhs.x, y - rhs.y); }
		CFG_FORCE_INLINE Vec operator *(const Vec& rhs) const { return Vec(x * rhs.x, y * rhs.y); }
		CFG_FORCE_INLINE Vec operator *(const T& rhs) const { return Vec(x * rhs, y * rhs); }
		CFG_FORCE_INLINE Vec operator /(const Vec& rhs) const { return Vec(x / rhs.x, y / rhs.y); }
		CFG_FORCE_INLINE Vec operator /(const T& rhs) const { return Vec(x / rhs, y / rhs); }
	};

	template <class T>
	CFG_FORCE_INLINE Vec<2, T> operator *(const T& lhs, const Vec<2, T>& rhs) { return rhs * lhs; }
}
