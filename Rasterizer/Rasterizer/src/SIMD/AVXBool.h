#pragma once

#include <immintrin.h>

#include "../Config.h"

class AVXBool
{
public:
	enum { size = 8 };

	union
	{
		__m256 m256;
		float v[8];
		int i[8];
	};

	CFG_FORCE_INLINE AVXBool() = default;
	CFG_FORCE_INLINE AVXBool(const AVXBool& copyFrom): m256(copyFrom.m256) {}
	CFG_FORCE_INLINE AVXBool(const __m256& val): m256(val) {}

	CFG_FORCE_INLINE AVXBool& operator =(const AVXBool& copyFrom)
	{
		m256 = copyFrom.m256;
		return *this;
	}

	CFG_FORCE_INLINE AVXBool(bool a)
	{
		auto True = _mm256_castsi256_ps(_mm256_cmpeq_epi32(_mm256_setzero_si256(), _mm256_setzero_si256()));
		auto False = _mm256_setzero_ps();

		m256 = a ? True : False;
	}

	CFG_FORCE_INLINE operator const __m256&(void) const { return m256; }
	CFG_FORCE_INLINE operator const __m256i(void) const { return _mm256_castps_si256(m256); }
	CFG_FORCE_INLINE operator const __m256d(void) const { return _mm256_castps_pd(m256); }

	CFG_FORCE_INLINE AVXBool operator !() const { return _mm256_xor_ps(*this, AVXBool(true)); }
	CFG_FORCE_INLINE AVXBool operator &(const AVXBool& rhs) const { return _mm256_and_ps(*this, rhs); }
	CFG_FORCE_INLINE AVXBool operator |(const AVXBool& rhs) const { return _mm256_or_ps(*this, rhs); }
	CFG_FORCE_INLINE AVXBool operator ^(const AVXBool& rhs) const { return _mm256_xor_ps(*this, rhs); }

	CFG_FORCE_INLINE bool operator [](const size_t i) const { return (_mm256_movemask_ps(m256) >> i) & 1; }
	CFG_FORCE_INLINE int32_t& operator [](const size_t id) { return i[id]; }
};
