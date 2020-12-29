#pragma once

#include <smmintrin.h>

#include "../Config.h"

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

	CFG_FORCE_INLINE SSEFloat(): m128(_mm_setzero_ps()) {}
	CFG_FORCE_INLINE SSEFloat(const SSEFloat& copyFrom): m128(copyFrom.m128) {}
	CFG_FORCE_INLINE SSEFloat(const __m128& val) : m128(val) {}
	CFG_FORCE_INLINE SSEFloat(const __m128i rhs): m128(_mm_cvtepi32_ps(rhs)) {}
	CFG_FORCE_INLINE SSEFloat(float a, float b, float c, float d): m128(_mm_set_ps(d, c, b, a)) {}

	CFG_FORCE_INLINE SSEFloat(const float& fVal) : m128(
		_mm_castsi128_ps(_mm_shuffle_epi32(_mm_castps_si128(_mm_load_ss(&fVal)), _MM_SHUFFLE(0, 0, 0, 0)))) {}

	CFG_FORCE_INLINE SSEFloat& operator =(const SSEFloat& copyFrom)
	{
		m128 = copyFrom.m128;
		return *this;
	}

	CFG_FORCE_INLINE operator const __m128&(void) const { return m128; }
	CFG_FORCE_INLINE operator __m128&(void) { return m128; }

	CFG_FORCE_INLINE const float& operator [](const size_t i) const { return v[i]; }
	CFG_FORCE_INLINE float& operator [](const size_t i) { return v[i]; }

	CFG_FORCE_INLINE SSEFloat operator +(const SSEFloat& rhs) const { return _mm_add_ps(m128, rhs); }
	CFG_FORCE_INLINE SSEFloat operator +(const float& rhs) const { return *this + SSEFloat(rhs); }
	CFG_FORCE_INLINE SSEFloat operator -(const SSEFloat& rhs) const { return _mm_sub_ps(m128, rhs); }
	CFG_FORCE_INLINE SSEFloat operator -(const float& rhs) const { return *this - SSEFloat(rhs); }
	CFG_FORCE_INLINE SSEFloat operator *(const SSEFloat& rhs) const { return _mm_mul_ps(m128, rhs); }
	CFG_FORCE_INLINE SSEFloat operator *(const float& rhs) const { return *this * SSEFloat(rhs); }
	CFG_FORCE_INLINE SSEFloat operator /(const SSEFloat& rhs) const { return *this * _mm_rcp_ps(rhs); }
	CFG_FORCE_INLINE SSEFloat operator /(const float& rhs) const { return *this * (1.0f / rhs); }

	CFG_FORCE_INLINE SSEBool operator >=(const SSEFloat& rhs) const { return _mm_cmpnlt_ps(m128, rhs.m128); }
	CFG_FORCE_INLINE SSEBool operator >(const SSEFloat& rhs) const { return _mm_cmpnle_ps(m128, rhs.m128); }
	CFG_FORCE_INLINE SSEBool operator >(const float& rhs) const { return *this > SSEFloat(rhs); }
	CFG_FORCE_INLINE SSEBool operator <(const SSEFloat& rhs) const { return _mm_cmplt_ps(m128, rhs.m128); }
	CFG_FORCE_INLINE SSEBool operator <(const float& rhs) const { return *this < SSEFloat(rhs); }
	CFG_FORCE_INLINE SSEBool operator >=(const float& rhs) const { return *this >= SSEFloat(rhs); }
	CFG_FORCE_INLINE SSEBool operator <=(const SSEFloat& rhs) const { return _mm_cmple_ps(m128, rhs.m128); }
	CFG_FORCE_INLINE SSEBool operator <=(const float& rhs) const { return *this <= SSEFloat(rhs); }
};
