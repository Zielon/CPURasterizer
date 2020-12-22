#pragma once

#include <immintrin.h>

#include "AVXCmp.h"

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

	__forceinline AVXFloat(const float& a, const float& b, const float& c, const float& d,
	                       const float& e, const float& f, const float& g, const float& h) : m256(
		_mm256_set_ps(h, g, f, e, d, c, b, a)) {}

	__forceinline AVXFloat& operator =(const AVXFloat& copyFrom)
	{
		m256 = copyFrom.m256;
		return *this;
	}

	__forceinline const float& operator [](const size_t i) const { return v[i]; }
	__forceinline float& operator [](const size_t i) { return v[i]; }

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

	__forceinline AVXBool operator >=(const AVXFloat& rhs) const { return AVX::_mm256_cmpnlt_ps(m256, rhs); }
	__forceinline AVXBool operator >(const AVXFloat& rhs) const { return AVX::_mm256_cmpnle_ps(m256, rhs); }
	__forceinline AVXBool operator >(const float& rhs) const { return *this > AVXFloat(rhs); }
	__forceinline AVXBool operator <(const AVXFloat& rhs) const { return AVX::_mm256_cmplt_ps(m256, rhs.m256); }
	__forceinline AVXBool operator <(const float& rhs) const { return *this < AVXFloat(rhs); }
	__forceinline AVXBool operator >=(const float& rhs) const { return *this >= AVXFloat(rhs); }
	__forceinline AVXBool operator <=(const AVXFloat& rhs) const { return AVX::_mm256_cmple_ps(m256, rhs); }
	__forceinline AVXBool operator <=(const float& rhs) const { return *this <= AVXFloat(rhs); }

	__forceinline AVXFloat& operator +=(const AVXFloat& rhs) { return *this = *this + rhs; }
	__forceinline AVXFloat& operator +=(const float& rhs) { return *this = *this + rhs; }
	__forceinline AVXFloat& operator -=(const AVXFloat& rhs) { return *this = *this - rhs; }
	__forceinline AVXFloat& operator -=(const float& rhs) { return *this = *this - rhs; }
};
