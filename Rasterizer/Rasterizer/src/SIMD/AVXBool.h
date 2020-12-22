#pragma once

#include <immintrin.h>

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

	__forceinline AVXBool() = default;
	__forceinline AVXBool(const AVXBool& copyFrom): m256(copyFrom.m256) {}
	__forceinline AVXBool(const __m256& val): m256(val) {}

	__forceinline AVXBool& operator =(const AVXBool& copyFrom)
	{
		m256 = copyFrom.m256;
		return *this;
	}

	__forceinline AVXBool(bool a)
	{
		auto True = _mm256_castsi256_ps(_mm256_cmpeq_epi32(_mm256_setzero_si256(), _mm256_setzero_si256()));
		auto False = _mm256_setzero_ps();

		m256 = a ? True : False;
	}

	__forceinline operator const __m256&(void) const { return m256; }
	__forceinline operator const __m256i(void) const { return _mm256_castps_si256(m256); }
	__forceinline operator const __m256d(void) const { return _mm256_castps_pd(m256); }

	__forceinline AVXBool operator !() const { return _mm256_xor_ps(*this, AVXBool(true)); }
	__forceinline AVXBool operator &(const AVXBool& rhs) const { return _mm256_and_ps(*this, rhs); }
	__forceinline AVXBool operator |(const AVXBool& rhs) const { return _mm256_or_ps(*this, rhs); }
	__forceinline AVXBool operator ^(const AVXBool& rhs) const { return _mm256_xor_ps(*this, rhs); }

	__forceinline bool operator [](const size_t i) const { return (_mm256_movemask_ps(m256) >> i) & 1; }
	__forceinline int32_t& operator [](const size_t id) { return i[id]; }
};
