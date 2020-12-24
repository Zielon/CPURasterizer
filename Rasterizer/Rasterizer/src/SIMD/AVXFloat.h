#pragma once

#include <immintrin.h>

#include "AVXCmp.h"

#include "../Config.h"

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

	CFG_FORCE_INLINE AVXFloat(): m256(_mm256_setzero_ps()) {}
	CFG_FORCE_INLINE AVXFloat(const AVXFloat& copyFrom): m256(copyFrom.m256) {}
	CFG_FORCE_INLINE AVXFloat(const __m256& val) : m256(val) {}
	CFG_FORCE_INLINE AVXFloat(const __m256i rhs): m256(_mm256_cvtepi32_ps(rhs)) {}
	CFG_FORCE_INLINE AVXFloat(const float& fVal) : m256(_mm256_broadcast_ss(&fVal)) {}

	CFG_FORCE_INLINE AVXFloat(const float& a, const float& b, const float& c, const float& d,
	                       const float& e, const float& f, const float& g, const float& h) : m256(
		_mm256_set_ps(h, g, f, e, d, c, b, a)) {}

	CFG_FORCE_INLINE AVXFloat& operator =(const AVXFloat& copyFrom)
	{
		m256 = copyFrom.m256;
		return *this;
	}

	CFG_FORCE_INLINE const float& operator [](const size_t i) const { return v[i]; }
	CFG_FORCE_INLINE float& operator [](const size_t i) { return v[i]; }

	CFG_FORCE_INLINE operator const __m256&(void) const { return m256; }
	CFG_FORCE_INLINE operator __m256&(void) { return m256; }

	CFG_FORCE_INLINE AVXFloat operator +(const AVXFloat& rhs) const { return _mm256_add_ps(m256, rhs); }
	CFG_FORCE_INLINE AVXFloat operator +(const float& rhs) const { return *this + AVXFloat(rhs); }
	CFG_FORCE_INLINE AVXFloat operator -(const AVXFloat& rhs) const { return _mm256_sub_ps(m256, rhs); }
	CFG_FORCE_INLINE AVXFloat operator -(const float& rhs) const { return *this - AVXFloat(rhs); }
	CFG_FORCE_INLINE AVXFloat operator *(const AVXFloat& rhs) const { return _mm256_mul_ps(m256, rhs); }
	CFG_FORCE_INLINE AVXFloat operator *(const float& rhs) const { return *this * AVXFloat(rhs); }
	CFG_FORCE_INLINE AVXFloat operator /(const AVXFloat& rhs) const { return *this * _mm256_rcp_ps(rhs); }
	CFG_FORCE_INLINE AVXFloat operator /(const float& rhs) const { return *this * (1.0f / rhs); }

	CFG_FORCE_INLINE AVXBool operator >=(const AVXFloat& rhs) const { return AVX::_mm256_cmpnlt_ps(m256, rhs); }
	CFG_FORCE_INLINE AVXBool operator >(const AVXFloat& rhs) const { return AVX::_mm256_cmpnle_ps(m256, rhs); }
	CFG_FORCE_INLINE AVXBool operator >(const float& rhs) const { return *this > AVXFloat(rhs); }
	CFG_FORCE_INLINE AVXBool operator <(const AVXFloat& rhs) const { return AVX::_mm256_cmplt_ps(m256, rhs.m256); }
	CFG_FORCE_INLINE AVXBool operator <(const float& rhs) const { return *this < AVXFloat(rhs); }
	CFG_FORCE_INLINE AVXBool operator >=(const float& rhs) const { return *this >= AVXFloat(rhs); }
	CFG_FORCE_INLINE AVXBool operator <=(const AVXFloat& rhs) const { return AVX::_mm256_cmple_ps(m256, rhs); }
	CFG_FORCE_INLINE AVXBool operator <=(const float& rhs) const { return *this <= AVXFloat(rhs); }

	CFG_FORCE_INLINE AVXFloat& operator +=(const AVXFloat& rhs) { return *this = *this + rhs; }
	CFG_FORCE_INLINE AVXFloat& operator +=(const float& rhs) { return *this = *this + rhs; }
	CFG_FORCE_INLINE AVXFloat& operator -=(const AVXFloat& rhs) { return *this = *this - rhs; }
	CFG_FORCE_INLINE AVXFloat& operator -=(const float& rhs) { return *this = *this - rhs; }
};
