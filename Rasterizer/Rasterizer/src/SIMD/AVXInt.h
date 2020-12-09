#pragma once

#include <immintrin.h>

class AVXInt
{
public:
	enum { size = 8 };

	union
	{
		__m256i m256;
		int i[8];
	};

	__forceinline AVXInt(): m256(_mm256_setzero_si256()) {}
	__forceinline AVXInt(const AVXInt& copyFrom): m256(copyFrom.m256) {}
	__forceinline AVXInt(const __m256i& val) : m256(val) {}
	__forceinline AVXInt(const uint32_t& a): m256(_mm256_castps_si256(_mm256_broadcast_ss((float*)&a))) {}
	__forceinline AVXInt(const __m256& val) : m256(_mm256_cvtps_epi32(val)) {}

	__forceinline AVXInt& operator =(const AVXInt& copyFrom)
	{
		m256 = copyFrom.m256;
		return *this;
	}

	__forceinline operator const __m256i&(void) const { return m256; }
	__forceinline operator __m256i&(void) { return m256; }

	__forceinline AVXInt operator +(const AVXInt& rhs) const { return _mm256_add_epi32(m256, rhs); }
	__forceinline AVXInt operator +(const uint32_t& rhs) const { return *this + AVXInt(rhs); }
	__forceinline AVXInt operator -(const AVXInt& rhs) const { return _mm256_sub_epi32(m256, rhs); }
	__forceinline AVXInt operator -(const uint32_t& rhs) const { return *this - AVXInt(rhs); }
	__forceinline AVXInt operator *(const AVXInt& rhs) const { return _mm256_mullo_epi32(m256, rhs); }
	__forceinline AVXInt operator *(const uint32_t& rhs) const { return *this * AVXInt(rhs); }
	__forceinline AVXInt operator &(const AVXInt& rhs) const { return _mm256_and_si256(m256, rhs); }
	__forceinline AVXInt operator &(const uint32_t& rhs) const { return *this & AVXInt(rhs); }
	__forceinline AVXInt operator |(const AVXInt& rhs) const { return _mm256_or_si256(m256, rhs); }
	__forceinline AVXInt operator |(const uint32_t& rhs) const { return *this | AVXInt(rhs); }
	__forceinline AVXBool operator >=(const AVXInt& rhs) const { return !(*this < rhs); }
	__forceinline AVXBool operator >=(const uint32_t& rhs) const { return *this >= AVXInt(rhs); }

	__forceinline AVXBool operator ==(const AVXInt& rhs) const
	{
		return _mm256_castsi256_ps(_mm256_cmpeq_epi32(m256, rhs));
	}

	__forceinline AVXBool operator ==(const uint32_t& rhs) const { return *this == AVXInt(rhs); }

	__forceinline AVXBool operator <(const AVXInt& rhs) const
	{
		return _mm256_castsi256_ps(_mm256_cmplt_epi32(m256, rhs));
	}

	__forceinline AVXBool operator <(const uint32_t& rhs) const { return *this < AVXInt(rhs); }

	__forceinline AVXBool operator >(const AVXInt& rhs) const
	{
		return _mm256_castsi256_ps(_mm256_cmpgt_epi32(m256, rhs));
	}

	__forceinline AVXBool operator >(const uint32_t& rhs) const { return *this > AVXInt(rhs); }

private:
	__forceinline __m256i _mm256_cmplt_epi32(__m256i a, __m256i b) const { return _mm256_cmpgt_epi32(b, a); }
};
