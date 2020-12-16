#pragma once

#include <smmintrin.h>

class SSEInt;
class SSEBool;

class SSEFloat
{
public:
	enum { size = 4 };

	union
	{
		__m128 m128;
		float v[4];
		int i[4];
	};

	__forceinline SSEFloat(): m128(_mm_setzero_ps()) {}
	__forceinline SSEFloat(const SSEFloat& copyFrom): m128(copyFrom.m128) {}
	__forceinline SSEFloat(const __m128& val) : m128(val) {}
	__forceinline SSEFloat(const __m128i rhs): m128(_mm_cvtepi32_ps(rhs)) {}

	__forceinline SSEFloat(const float& fVal) : m128(
		_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(_mm_load_ss(&fVal)), _MM_SHUFFLE(0, 0, 0, 0)))) {}

	__forceinline SSEFloat& operator =(const SSEFloat& copyFrom)
	{
		m128 = copyFrom.m128;
		return *this;
	}

	__forceinline operator const __m128&(void) const { return m128; }
	__forceinline operator __m128&(void) { return m128; }

	__forceinline const float& operator [](const size_t i) const { return v[i]; }
	__forceinline float& operator [](const size_t i) { return v[i]; }

	__forceinline SSEFloat operator +(const SSEFloat& rhs) const { return _mm_add_ps(m128, rhs); }
	__forceinline SSEFloat operator +(const float& rhs) const { return *this + SSEFloat(rhs); }
	__forceinline SSEFloat operator -(const SSEFloat& rhs) const { return _mm_sub_ps(m128, rhs); }
	__forceinline SSEFloat operator -(const float& rhs) const { return *this - SSEFloat(rhs); }
	__forceinline SSEFloat operator *(const SSEFloat& rhs) const { return _mm_mul_ps(m128, rhs); }
	__forceinline SSEFloat operator *(const float& rhs) const { return *this * SSEFloat(rhs); }
	__forceinline SSEFloat operator /(const SSEFloat& rhs) const { return *this * _mm_rcp_ps(rhs); }
	__forceinline SSEFloat operator /(const float& rhs) const { return *this * (1.0f / rhs); }

	__forceinline SSEBool operator >=(const SSEFloat& rhs) const { return _mm_cmpnlt_ps(m128, rhs.m128); }
	__forceinline SSEBool operator >(const SSEFloat& rhs) const { return _mm_cmpnle_ps(m128, rhs.m128); }
	__forceinline SSEBool operator >(const float& rhs) const { return *this > SSEFloat(rhs); }
	__forceinline SSEBool operator <(const SSEFloat& rhs) const { return _mm_cmplt_ps(m128, rhs.m128); }
	__forceinline SSEBool operator <(const float& rhs) const { return *this < SSEFloat(rhs); }
	__forceinline SSEBool operator >=(const float& rhs) const { return *this >= SSEFloat(rhs); }
	__forceinline SSEBool operator <=(const SSEFloat& rhs) const { return _mm_cmple_ps(m128, rhs.m128); }
	__forceinline SSEBool operator <=(const float& rhs) const { return *this <= SSEFloat(rhs); }
};