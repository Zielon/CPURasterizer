#pragma once

#include <immintrin.h>

namespace Math
{
	inline float Rsqrt(const float x)
	{
		const __m128 a = _mm_set_ss(x);
		const __m128 r = _mm_rsqrt_ps(a);
		const __m128 c = _mm_add_ps(_mm_mul_ps(_mm_set_ps(1.5f, 1.5f, 1.5f, 1.5f), r),
		                            _mm_mul_ps(_mm_mul_ps(_mm_mul_ps(a, _mm_set_ps(-0.5f, -0.5f, -0.5f, -0.5f)), r),
		                                       _mm_mul_ps(r, r)));
		return _mm_cvtss_f32(c);
	}

	template <class T1, class T2>
	__forceinline auto Max(const T1& e1, const T2& e2) -> decltype(e1 + e2) { return e1 > e2 ? e1 : e2; }

	template <class T1, class T2>
	__forceinline auto AbsMax(const T1& e1, const T2& e2) -> decltype(e1 + e2) { return Abs(e1) > Abs(e2) ? e1 : e2; }

	template <class T1, class T2>
	__forceinline auto Min(const T1& e1, const T2& e2) -> decltype(e1 + e2) { return e1 < e2 ? e1 : e2; }

	template <class T1, class T2>
	__forceinline auto AbsMin(const T1& e1, const T2& e2) -> decltype(e1 + e2) { return Abs(e1) < Abs(e2) ? e1 : e2; }

	template <class T1, class T2, class T3>
	__forceinline T1 Clamp(const T1& tVal, const T2& tMin, const T3& tMax)
	{
		if (tVal < tMin) return tMin;
		if (tVal > tMax) return tMax;
		return tVal;
	}
}
