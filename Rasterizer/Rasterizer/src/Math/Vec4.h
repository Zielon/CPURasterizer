#pragma once

#include "Vec.h"

namespace Math
{
	template <class T>
	class Vec<4, T>
	{
	public:
		T x{}, y{}, z{}, w{};

		__forceinline Vec() = default;
		__forceinline Vec(const Vec& vec): x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}
		__forceinline Vec(const T& tVal) : x(tVal), y(tVal), z(tVal), w(tVal) {}
		__forceinline Vec(const T& tx, const T& ty, const T& tz, const T& tw): x(tx), y(ty), z(tz), w(tw) {}

		__forceinline Vec& operator =(const Vec& vec)
		{
			x = vec.x;
			y = vec.y;
			z = vec.z;
			w = vec.w;

			return *this;
		}

		~Vec() {}

		__forceinline T Sum() const { return x + y + z + w; }
		__forceinline T Product() const { return x * y * z * w; }
		__forceinline Vec<3, T> xyz() const { return Vec<3, T>(x, y, z); }
		__forceinline Vec<3, T> HomogeneousProject() const
		{
			float invW = 1.0f / w;
			return Vec<3, T>(x * invW, y * invW, z * invW);
		}

		__forceinline const T& operator [] (const size_t idx) const { return (&x)[idx]; }
		__forceinline		T& operator [] (const size_t idx)		{ return (&x)[idx]; }

		__forceinline Vec operator + () const { return Vec(+x, +y, +z, +w); }
		__forceinline Vec operator - () const { return Vec(-x, -y, -z, -w); }

		__forceinline Vec operator + (const Vec& rhs) const { return Vec(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }
		__forceinline Vec operator - (const Vec& rhs) const { return Vec(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); }
		__forceinline Vec operator * (const Vec& rhs) const { return Vec(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w); }
		__forceinline Vec operator * (const T& rhs) const { return Vec(x * rhs, y * rhs, z * rhs, w * rhs); }
		__forceinline Vec operator / (const Vec& rhs) const { return Vec(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w); }
		__forceinline Vec operator / (const T& rhs) const { return Vec(x / rhs, y / rhs, z / rhs, w / rhs); }
		__forceinline Vec operator << (const int shift) const { return Vec(x << shift, y << shift, z << shift, w << shift); }
		__forceinline Vec operator >> (const int shift) const { return Vec(x >> shift, y >> shift, z >> shift, w >> shift); }
		__forceinline Vec operator & (const int bits) const { return Vec(x & bits, y & bits, z & bits, w & bits); }
		__forceinline Vec operator | (const int bits) const { return Vec(x | bits, y | bits, z | bits, w | bits); }

		__forceinline const Vec& operator += (const Vec& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
		__forceinline const Vec& operator -= (const Vec& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }
		__forceinline const Vec& operator *= (const T& rhs) { x *= rhs; y *= rhs; z *= rhs; w *= rhs; return *this; }
		__forceinline const Vec& operator /= (const T& rhs) { x /= rhs; y /= rhs; z /= rhs; w /= rhs; return *this; }
		__forceinline const Vec& operator *= (const Vec& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; }
		__forceinline const Vec& operator /= (const Vec& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; w /= rhs.w; return *this; }
		__forceinline const Vec& operator <<= (const int shift) { x <<= shift; y <<= shift; z <<= shift; w <<= shift; return *this; }
		__forceinline const Vec& operator >>= (const int shift) { x >>= shift; y >>= shift; z >>= shift; w >>= shift; return *this; }
		__forceinline const Vec& operator &= (const int bits) const { x &= bits, y &= bits; z &= bits; w &= bits; return *this; }
		__forceinline const Vec& operator |= (const int bits) const { x |= bits, y |= bits; z &= bits; w |= bits; return *this; }

		__forceinline bool operator == (const Vec& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
		__forceinline bool operator != (const Vec& rhs) const { return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w; }

	};

	using Vector4 = Vec<4, float>;
	using Vector4i = Vec<4, int>;
	using Vector4b = Vec<4, bool>;
	
	template <class T>
	__forceinline Vec<4, T> operator *(const T& lhs, const Vec<4, T>& rhs) { return rhs * lhs; }

	template <class T>
	__forceinline T Min(const Vec<4, T>& v) { return Math::Min(Math::Min(Math::Min(v.x, v.y), v.z), v.w); }

	template <class T>
	__forceinline T Max(const Vec<4, T>& v) { return Math::Max(Math::Max(Math::Max(v.x, v.y), v.z), v.w); }
}