#pragma once

#include <cstdint>
#include <smmintrin.h>
#include <immintrin.h>

#include "../Config.h"

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

	CFG_FORCE_INLINE SSEInt(): m128(_mm_setzero_si128()) {}
	CFG_FORCE_INLINE SSEInt(const SSEInt& copyFrom): m128(copyFrom.m128) {}
	CFG_FORCE_INLINE SSEInt(const __m128i& val) : m128(val) {}
	CFG_FORCE_INLINE SSEInt(uint32_t a, uint32_t b) : m128(_mm_set_epi32(b, a, b, a)) {}
	CFG_FORCE_INLINE SSEInt(uint32_t a, uint32_t b, uint32_t c, uint32_t d) : m128(_mm_set_epi32(d, c, b, a)) {}
	CFG_FORCE_INLINE SSEInt(const uint32_t& a): m128(_mm_castps_si128(_mm_broadcast_ss((const float*)&a))) {}

	CFG_FORCE_INLINE SSEInt& operator =(const SSEInt& copyFrom)
	{
		m128 = copyFrom.m128;
		return *this;
	}

	CFG_FORCE_INLINE operator const __m128i&(void) const { return m128; }
	CFG_FORCE_INLINE operator __m128i&(void) { return m128; }

	CFG_FORCE_INLINE SSEInt operator +(const SSEInt& rhs) const { return _mm_add_epi32(m128, rhs.m128); }
	CFG_FORCE_INLINE SSEInt operator +(const uint32_t& rhs) const { return *this + SSEInt(rhs); }
	CFG_FORCE_INLINE SSEInt operator -(const SSEInt& rhs) const { return _mm_sub_epi32(m128, rhs.m128); }
	CFG_FORCE_INLINE SSEInt operator -(const uint32_t& rhs) const { return *this - SSEInt(rhs); }
	CFG_FORCE_INLINE SSEInt operator *(const SSEInt& rhs) const { return _mm_mullo_epi32(m128, rhs.m128); }
	CFG_FORCE_INLINE SSEInt operator *(const uint32_t& rhs) const { return *this * SSEInt(rhs); }
	CFG_FORCE_INLINE SSEInt& operator +=(const SSEInt& rhs) { return *this = *this + rhs; }
	CFG_FORCE_INLINE SSEInt& operator +=(const uint32_t& rhs) { return *this = *this + rhs; }
	CFG_FORCE_INLINE SSEInt operator &(const SSEInt& rhs) const { return _mm_and_si128(m128, rhs.m128); }
	CFG_FORCE_INLINE SSEInt operator &(const uint32_t& rhs) const { return *this & SSEInt(rhs); }
	CFG_FORCE_INLINE SSEInt operator |(const SSEInt& rhs) const { return _mm_or_si128(m128, rhs.m128); }
	CFG_FORCE_INLINE SSEInt operator |(const uint32_t& rhs) const { return *this | SSEInt(rhs); }
	CFG_FORCE_INLINE SSEBool operator >=(const SSEInt& rhs) const { return !(*this < rhs); }
	CFG_FORCE_INLINE SSEBool operator >=(const uint32_t& rhs) const { return *this >= SSEInt(rhs); }

	CFG_FORCE_INLINE SSEBool operator ==(const SSEInt& rhs) const
	{
		return _mm_castsi128_ps(_mm_cmpeq_epi32(m128, rhs.m128));
	}

	CFG_FORCE_INLINE SSEBool operator ==(const uint32_t& rhs) const { return *this == SSEInt(rhs); }

	CFG_FORCE_INLINE SSEBool operator <(const SSEInt& rhs) const
	{
		return _mm_castsi128_ps(_mm_cmplt_epi32(m128, rhs.m128));
	}

	CFG_FORCE_INLINE SSEBool operator <(const uint32_t& rhs) const { return *this < SSEInt(rhs); }

	CFG_FORCE_INLINE SSEBool operator >(const SSEInt& rhs) const
	{
		return _mm_castsi128_ps(_mm_cmpgt_epi32(m128, rhs.m128));
	}

	CFG_FORCE_INLINE SSEBool operator >(const uint32_t& rhs) const { return *this > SSEInt(rhs); }
};
