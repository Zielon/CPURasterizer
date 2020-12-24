#pragma once

#include "Vec.h"

#include "../Config.h"

namespace Math
{
	template <class T>
	class Vec<3, T>
	{
	public:
		T x{}, y{}, z{};

		CFG_FORCE_INLINE Vec() = default;
		CFG_FORCE_INLINE Vec(const Vec& vec): x(vec.x), y(vec.y), z(vec.z) { }
		CFG_FORCE_INLINE Vec(const T& tVal): x(tVal), y(tVal), z(tVal) { }
		CFG_FORCE_INLINE Vec(const T& tx, const T& ty, const T& tz) : x(tx), y(ty), z(tz) { }
		CFG_FORCE_INLINE Vec(const glm::vec<3, T>& vec) : x(vec.x), y(vec.y), z(vec.z) {}

		CFG_FORCE_INLINE Vec& operator =(const Vec& vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
			return *this;
		}

		~Vec() = default;

		[[nodiscard]] CFG_FORCE_INLINE T Sum() const { return x + y + z; }
		[[nodiscard]] CFG_FORCE_INLINE T Product() const { return x * y * z; }

		CFG_FORCE_INLINE const T& operator [](const size_t idx) const { return (&x)[idx]; }
		CFG_FORCE_INLINE T& operator [](const size_t idx) { return (&x)[idx]; }

		CFG_FORCE_INLINE Vec operator +(const Vec& rhs) const { return Vec(x + rhs.x, y + rhs.y, z + rhs.z); }
		CFG_FORCE_INLINE Vec operator -(const Vec& rhs) const { return Vec(x - rhs.x, y - rhs.y, z - rhs.z); }
		CFG_FORCE_INLINE Vec operator /(const Vec& rhs) const { return Vec(x / rhs.x, y / rhs.y, z / rhs.z); }
		CFG_FORCE_INLINE Vec operator /(const T& rhs) const { return Vec(x / rhs, y / rhs, z / rhs); }
		CFG_FORCE_INLINE Vec operator *(const Vec& rhs) const { return Vec(x * rhs.x, y * rhs.y, z * rhs.z); }
		CFG_FORCE_INLINE Vec operator *(const T& rhs) const { return Vec(x * rhs, y * rhs, z * rhs); }

		CFG_FORCE_INLINE Vec operator *(const glm::vec<3, float>& rhs) const
		{
			return Vec(x * rhs.x, y * rhs.y, z * rhs.z);
		}

		CFG_FORCE_INLINE const Vec& operator +=(const Vec& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			z += rhs.z;
			
			return *this;
		}

		CFG_FORCE_INLINE const Vec& operator -=(const Vec& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			z -= rhs.z;
			
			return *this;
		}
	};

	template <class T>
	CFG_FORCE_INLINE Vec<3, T> operator *(const T& lhs, const Vec<3, T>& rhs) { return rhs * lhs; }

	template <class T>
	CFG_FORCE_INLINE T Dot(const Vec<3, T>& vec1, const Vec<3, T>& vec2)
	{
		return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
	}

	template <class T>
	CFG_FORCE_INLINE T AbsDot(const Vec<3, T>& vec1, const Vec<3, T>& vec2)
	{
		T ret = Dot(vec1, vec2);
		return ret >= 0 ? ret : -ret;
	}

	template <class T>
	CFG_FORCE_INLINE Vec<3, T> Cross(const Vec<3, T>& vec1, const Vec<3, T>& vec2)
	{
		return Vec<3, T>(vec1.y * vec2.z - vec1.z * vec2.y,
		                 vec1.z * vec2.x - vec1.x * vec2.z,
		                 vec1.x * vec2.y - vec1.y * vec2.x);
	}

	template <class T>
	CFG_FORCE_INLINE T LengthSquared(const Vec<3, T>& vec)
	{
		return Dot(vec, vec);
	}

	template <class T>
	CFG_FORCE_INLINE Vec<3, T> Reflect(const Vec<3, T>& v, const Vec<3, T>& n)
	{
		return v + Vec<3, T>(Dot(v * -1.f, n) * n * 2);
	}
}
