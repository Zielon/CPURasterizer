#pragma once

#include "../SIMD/SSE.h"

namespace Math
{
	__forceinline const SSEFloat Rsqrt(const SSEFloat& rhs)
	{
		const SSEFloat r = _mm_rsqrt_ps(rhs.m128);
		return _mm_add_ps(_mm_mul_ps(_mm_set_ps(1.5f, 1.5f, 1.5f, 1.5f), r),
		                  _mm_mul_ps(_mm_mul_ps(_mm_mul_ps(rhs, _mm_set_ps(-0.5f, -0.5f, -0.5f, -0.5f)), r),
		                             _mm_mul_ps(r, r)));
	}

	__forceinline const AVXFloat Rsqrt(const AVXFloat& rhs)
	{
		const AVXFloat r = _mm256_rsqrt_ps(rhs.m256);
		return _mm256_add_ps(_mm256_mul_ps(_mm256_set_ps(1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f), r),
		                     _mm256_mul_ps(
			                     _mm256_mul_ps(_mm256_mul_ps(
				                                   rhs, _mm256_set_ps(-0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
				                                                      -0.5f)), r),
			                     _mm256_mul_ps(r, r)));
	}

	template <class T>
	__forceinline T Dot(const Vec<3, T>& vec1, const Vec<3, T>& vec2)
	{
		return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
	}
}
