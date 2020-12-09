#pragma once

#include <immintrin.h>

class AVXInt;
class AVXBool;

class AVXFloat
{
public:
	enum { size = 8 };

	union
	{
		__m256 m256;
		float v[8];
		int i[8];
	};

	__forceinline AVXFloat(): m256(_mm256_setzero_ps()) {}
	__forceinline AVXFloat(const AVXFloat& copyFrom): m256(copyFrom.m256) {}
	__forceinline AVXFloat(const __m256& val) : m256(val) {}
	__forceinline AVXFloat(const __m256i rhs): m256(_mm256_cvtepi32_ps(rhs)) {}
	__forceinline AVXFloat(const float& fVal) : m256(_mm256_broadcast_ss(&fVal)) {}

	__forceinline AVXFloat& operator =(const AVXFloat& copyFrom)
	{
		m256 = copyFrom.m256;
		return *this;
	}

	__forceinline operator const __m256&(void) const { return m256; }
	__forceinline operator __m256&(void) { return m256; }

	__forceinline AVXFloat operator +(const AVXFloat& rhs) const { return _mm256_add_ps(m256, rhs); }
	__forceinline AVXFloat operator +(const float& rhs) const { return *this + AVXFloat(rhs); }
	__forceinline AVXFloat operator -(const AVXFloat& rhs) const { return _mm256_sub_ps(m256, rhs); }
	__forceinline AVXFloat operator -(const float& rhs) const { return *this - AVXFloat(rhs); }
	__forceinline AVXFloat operator *(const AVXFloat& rhs) const { return _mm256_mul_ps(m256, rhs); }
	__forceinline AVXFloat operator *(const float& rhs) const { return *this * AVXFloat(rhs); }
	__forceinline AVXFloat operator /(const AVXFloat& rhs) const { return *this * _mm256_rcp_ps(rhs); }
	__forceinline AVXFloat operator /(const float& rhs) const { return *this * (1.0f / rhs); }
};
