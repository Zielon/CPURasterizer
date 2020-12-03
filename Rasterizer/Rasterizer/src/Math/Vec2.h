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
		__forceinline Vec operator <<(const int shift) const { return Vec(x << shift, y << shift); }
		__forceinline Vec operator >>(const int shift) const { return Vec(x >> shift, y >> shift); }
		__forceinline Vec operator &(const int bits) const { return Vec(x & bits, y & bits); }
		__forceinline Vec operator |(const int bits) const { return Vec(x | bits, y | bits); }

		__forceinline const Vec& operator += (const Vec& rhs) { x += rhs.x; y += rhs.y; return *this; }
		__forceinline const Vec& operator -= (const Vec& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
		__forceinline const Vec& operator *= (const T& rhs) { x *= rhs; y *= rhs; return *this; }
		__forceinline const Vec& operator /= (const T& rhs) { x /= rhs; y /= rhs; return *this; }
		__forceinline const Vec& operator *= (const Vec& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
		__forceinline const Vec& operator /= (const Vec& rhs) { x /= rhs.x; y /= rhs.y; return *this; }
		__forceinline const Vec& operator <<= (const int shift) { x <<= shift; y <<= shift; return *this; }
		__forceinline const Vec& operator >>= (const int shift) { x >>= shift; y >>= shift; return *this; }
		__forceinline const Vec& operator &= (const int bits) const { x &= bits, y &= bits; return *this; }
		__forceinline const Vec& operator |= (const int bits) const { x |= bits, y |= bits; return *this; }
		
		__forceinline bool operator ==(const Vec& rhs) const { return x == rhs.x && y == rhs.y; }
		__forceinline bool operator !=(const Vec& rhs) const { return x != rhs.x || y != rhs.y; }
	};

	using Vector2 = Vec<2, float>;
	using Vector2i = Vec<2, int>;
	using Vector2b = Vec<2, bool>;

	template <class T>
	__forceinline Vec<2, T> operator *(const T& lhs, const Vec<2, T>& rhs) { return rhs * lhs; }

	template <class T>
	__forceinline T Dot(const Vec<2, T>& vec1, const Vec<2, T>& vec2) { return vec1.x * vec2.x + vec1.y * vec2.y; }

	template <class T>
	__forceinline T AbsDot(const Vec<2, T>& vec1, const Vec<2, T>& vec2)
	{
		T ret = Dot(vec1, vec2);
		return ret >= 0 ? ret : -ret;
	}

	template <class T>
	__forceinline T Cross(const Vec<2, T>& vec1, const Vec<2, T>& vec2) { return vec1.x * vec2.y - vec1.y * vec2.x; }

	template <class T>
	__forceinline T LengthSquared(const Vec<2, T>& vec) { return Dot(vec, vec); }

	template <class T>
	__forceinline float Length(const Vec<2, T>& vec) { return sqrtf(LengthSquared(vec)); }

	template <class T>
	__forceinline T Min(const Vec<2, T>& v) { return std::min(v.x, v.y); }

	template <class T>
	__forceinline T Max(const Vec<2, T>& v) { return std::max(v.x, v.y); }

	__forceinline Vector2 Normalize(const Vector2& v) { return v / Length(v); }
	__forceinline float Distance(const Vector2& p1, const Vector2& p2) { return Length(p1 - p2); }
	__forceinline float DistanceSquared(const Vector2& p1, const Vector2& p2) { return LengthSquared(p1 - p2); }	
}
