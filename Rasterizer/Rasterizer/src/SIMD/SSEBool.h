#pragma once

#include <smmintrin.h>

class SSEBool
{
public:
	enum { size = 4 };

	union
	{
		__m128 m128;
		float v[4];
		int i[4];
	};

	__forceinline SSEBool() {}
	__forceinline SSEBool(const SSEBool& copyFrom): m128(copyFrom.m128) {}
	__forceinline SSEBool(const __m128& val): m128(val) {}

	__forceinline SSEBool& operator =(const SSEBool& copyFrom)
	{
		m128 = copyFrom.m128;
		return *this;
	}

	__forceinline SSEBool(bool a)
	{
		auto True = _mm_castsi128_ps(_mm_cmpeq_epi32(_mm_setzero_si128(), _mm_setzero_si128()));
		auto False = _mm_setzero_ps();

		m128 = a ? True : False;
	}

	__forceinline operator const __m128&(void) const { return m128; }
	__forceinline operator const __m128i(void) const { return _mm_castps_si128(m128); }
	__forceinline operator const __m128d(void) const { return _mm_castps_pd(m128); }

	__forceinline SSEBool operator !() const { return _mm_xor_ps(*this, SSEBool(true)); }
	__forceinline SSEBool operator &(const SSEBool& rhs) const { return _mm_and_ps(*this, rhs); }
	__forceinline SSEBool operator |(const SSEBool& rhs) const { return _mm_or_ps(*this, rhs); }
	__forceinline SSEBool operator ^(const SSEBool& rhs) const { return _mm_xor_ps(*this, rhs); }

	__forceinline bool operator [](const size_t i) const { return (_mm_movemask_ps(m128) >> i) & 1; }
	__forceinline int32_t& operator [](const size_t id) { return i[id]; }
};
