#pragma once

#include "Math.h"
#include "Vec.h"

namespace Math
{
	template<class T>
	class Vec<3, T>
	{
	public:
		T x{}, y{}, z{};

		__forceinline Vec() = default;
		__forceinline Vec(const Vec& vec): x(vec.x), y(vec.y), z(vec.z) { }
		__forceinline Vec(const T& tVal): x(tVal), y(tVal), z(tVal) { }
		__forceinline Vec(const T& tx, const T& ty, const T& tz) : x(tx), y(ty), z(tz) { }

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

		__forceinline const T& operator [] (const size_t idx) const { return (&x)[idx]; }
		__forceinline		T& operator [] (const size_t idx)		{ return (&x)[idx]; }
		__forceinline Vec operator + () const { return Vec(+x, +y, +z); }
		__forceinline Vec operator - () const { return Vec(-x, -y, -z); }

		__forceinline Vec operator + (const Vec& rhs) const { return Vec(x + rhs.x, y + rhs.y, z + rhs.z); }
		__forceinline Vec operator - (const Vec& rhs) const { return Vec(x - rhs.x, y - rhs.y, z - rhs.z); }
		__forceinline Vec operator * (const Vec& rhs) const { return Vec(x * rhs.x, y * rhs.y, z * rhs.z); }
		__forceinline Vec operator * (const T& rhs) const { return Vec(x * rhs, y * rhs, z * rhs); }
		__forceinline Vec operator / (const Vec& rhs) const { return Vec(x / rhs.x, y / rhs.y, z / rhs.z); }
		__forceinline Vec operator / (const T& rhs) const { return Vec(x / rhs, y / rhs, z / rhs); }
		__forceinline Vec operator << (const int shift) const { return Vec(x << shift, y << shift, z << shift); }
		__forceinline Vec operator >> (const int shift) const { return Vec(x >> shift, y >> shift, z >> shift); }
		__forceinline Vec operator & (const int bits) const { return Vec(x & bits, y & bits, z & bits); }
		__forceinline Vec operator | (const int bits) const { return Vec(x | bits, y | bits, z | bits); }

		__forceinline const Vec& operator += (const Vec& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
		__forceinline const Vec& operator -= (const Vec& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
		__forceinline const Vec& operator *= (const T& rhs) { x *= rhs; y *= rhs; z *= rhs; return *this; }
		__forceinline const Vec& operator /= (const T& rhs) { x /= rhs; y /= rhs; z /= rhs; return *this; }
		__forceinline const Vec& operator *= (const Vec& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
		__forceinline const Vec& operator /= (const Vec& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }
		__forceinline const Vec& operator <<= (const int shift) { x <<= shift; y <<= shift; z <<= shift; return *this; }
		__forceinline const Vec& operator >>= (const int shift) { x >>= shift; y >>= shift; z >>= shift; return *this; }
		__forceinline const Vec& operator &= (const int bits) const { x &= bits, y &= bits; z &= bits; return *this; }
		__forceinline const Vec& operator |= (const int bits) const { x |= bits, y |= bits; z &= bits; return *this; }

		__forceinline bool operator == (const Vec& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }
		__forceinline bool operator != (const Vec& rhs) const { return x != rhs.x || y != rhs.y || z != rhs.z; }
	};

	using Vector3 = Vec<3, float>;
	using Vector3d = Vec<3, double>;
	using Vector3i = Vec<3, int>;
	using Vector3b = Vec<3, bool>;

	template <class T>
	__forceinline Vec<3, T> operator *(const T& lhs, const Vec<3, T>& rhs) { return rhs * lhs; }

	template <class T>
	__forceinline T Dot(const Vec<3, T>& vec1, const Vec<3, T>& vec2)
	{
		return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
	}

	template <class T>
	__forceinline T AbsDot(const Vec<3, T>& vec1, const Vec<3, T>& vec2)
	{
		T ret = Dot(vec1, vec2);
		return ret >= 0 ? ret : -ret;
	}

	template <class T>
	__forceinline Vec<3, T> Cross(const Vec<3, T>& vec1, const Vec<3, T>& vec2)
	{
		return Vec<3, T>(vec1.y * vec2.z - vec1.z * vec2.y,
		                 vec1.z * vec2.x - vec1.x * vec2.z,
		                 vec1.x * vec2.y - vec1.y * vec2.x);
	}

	template <class T>
	__forceinline Vec<3, T> Curl(const Vec<3, T>& vDvdx, const Vec<3, T>& vDvdy, const Vec<3, T>& vDvdz)
	{
		return Vec<3, T>(vDvdy.z - vDvdz.y, vDvdz.x - vDvdx.z, vDvdx.y - vDvdy.x);
	}

	template <class T>
	__forceinline T LengthSquared(const Vec<3, T>& vec) { return Dot(vec, vec); }

	template <class T>
	__forceinline T Min(const Vec<3, T>& v) { return Math::Min(Math::Min(v.x, v.y), v.z); }

	template <class T>
	__forceinline T Max(const Vec<3, T>& v) { return Math::Max(Math::Max(v.x, v.y), v.z); }

	template <class T>
	__forceinline float Length(const Vec<3, T>& vec) { return sqrtf(LengthSquared(vec)); }

	__forceinline Vector3 Normalize(const Vector3& v) { return v * Rsqrt(LengthSquared(v)); }
	__forceinline float Distance(const Vector3& p1, const Vector3& p2) { return Length(p1 - p2); }
	__forceinline float DistanceSquared(const Vector3& p1, const Vector3& p2) { return LengthSquared(p1 - p2); }
}