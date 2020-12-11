#pragma once

#include <cstdint>
#include <smmintrin.h>
#include <immintrin.h>

class SSEBool;

class SSEInt
{
public:
	enum { size = 4 };

	union
	{
		__m128i m128;
		int i[4];
	};

	__forceinline SSEInt(): m128(_mm_setzero_si128()) {}
	__forceinline SSEInt(const SSEInt& copyFrom): m128(copyFrom.m128) {}
	__forceinline SSEInt(const __m128i& val) : m128(val) {}
	__forceinline SSEInt(uint32_t a, uint32_t b) : m128(_mm_set_epi32(b, a, b, a)) {}
	__forceinline SSEInt(uint32_t a, uint32_t b, uint32_t c, uint32_t d) : m128(_mm_set_epi32(d, c, b, a)) {}
	__forceinline SSEInt(const uint32_t& a): m128(_mm_castps_si128(_mm_broadcast_ss((const float*)&a))) {}

	__forceinline SSEInt& operator =(const SSEInt& copyFrom)
	{
		m128 = copyFrom.m128;
		return *this;
	}

	__forceinline operator const __m128i&(void) const { return m128; }
	__forceinline operator __m128i&(void) { return m128; }

	__forceinline SSEInt operator +(const SSEInt& rhs) const { return _mm_add_epi32(m128, rhs.m128); }
	__forceinline SSEInt operator +(const uint32_t& rhs) const { return *this + SSEInt(rhs); }
	__forceinline SSEInt operator -(const SSEInt& rhs) const { return _mm_sub_epi32(m128, rhs.m128); }
	__forceinline SSEInt operator -(const uint32_t& rhs) const { return *this - SSEInt(rhs); }
	__forceinline SSEInt operator *(const SSEInt& rhs) const { return _mm_mullo_epi32(m128, rhs.m128); }
	__forceinline SSEInt operator *(const uint32_t& rhs) const { return *this * SSEInt(rhs); }
	__forceinline SSEInt& operator +=(const SSEInt& rhs) { return *this = *this + rhs; }
	__forceinline SSEInt& operator +=(const uint32_t& rhs) { return *this = *this + rhs; }
	__forceinline SSEInt operator &(const SSEInt& rhs) const { return _mm_and_si128(m128, rhs.m128); }
	__forceinline SSEInt operator &(const uint32_t& rhs) const { return *this & SSEInt(rhs); }
	__forceinline SSEInt operator |(const SSEInt& rhs) const { return _mm_or_si128(m128, rhs.m128); }
	__forceinline SSEInt operator |(const uint32_t& rhs) const { return *this | SSEInt(rhs); }
	__forceinline SSEBool operator >=(const SSEInt& rhs) const { return !(*this < rhs); }
	__forceinline SSEBool operator >=(const uint32_t& rhs) const { return *this >= SSEInt(rhs); }

	__forceinline SSEBool operator ==(const SSEInt& rhs) const
	{
		return _mm_castsi128_ps(_mm_cmpeq_epi32(m128, rhs.m128));
	}

	__forceinline SSEBool operator ==(const uint32_t& rhs) const { return *this == SSEInt(rhs); }

	__forceinline SSEBool operator <(const SSEInt& rhs) const
	{
		return _mm_castsi128_ps(_mm_cmplt_epi32(m128, rhs.m128));
	}

	__forceinline SSEBool operator <(const uint32_t& rhs) const { return *this < SSEInt(rhs); }

	__forceinline SSEBool operator >(const SSEInt& rhs) const
	{
		return _mm_castsi128_ps(_mm_cmpgt_epi32(m128, rhs.m128));
	}

	__forceinline SSEBool operator >(const uint32_t& rhs) const { return *this > SSEInt(rhs); }
};
