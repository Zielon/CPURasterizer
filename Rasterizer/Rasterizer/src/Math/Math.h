#pragma once

constexpr float PI = 3.14159265358979323846f;
constexpr float INVPI = 0.31830988618379069122f;

#include "../SIMD/AVX.h"

namespace Math
{
	const __m256 absmask = _mm256_castsi256_ps(_mm256_set1_epi32(~(1 << 31)));

	__forceinline __m256 Abs(__m256 v)
	{
		return _mm256_and_ps(absmask, v);
	}

	__forceinline AVXVec3f Abs(AVXVec3f& vec)
	{
		AVXVec3f out;

		out.x = Abs(vec.x);
		out.y = Abs(vec.y);
		out.z = Abs(vec.z);

		return out;
	}

	__forceinline AVXFloat Max(const AVXFloat& lhs, const AVXFloat& rhs)
	{
		return _mm256_max_ps(lhs.m256, rhs.m256);
	}

	__forceinline AVXFloat Max(const AVXFloat& lhs, const float& rhs)
	{
		return _mm256_max_ps(lhs.m256, AVXFloat(rhs));
	}

	__forceinline AVXFloat Max(const float& lhs, const AVXFloat& rhs)
	{
		return _mm256_max_ps(AVXFloat(lhs), rhs.m256);
	}

	__forceinline const AVXFloat Rsqrt(const AVXFloat& rhs)
	{
		const AVXFloat r = _mm256_rsqrt_ps(rhs.m256);

		return _mm256_add_ps(
			_mm256_mul_ps(_mm256_set_ps(1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f, 1.5f), r),
			_mm256_mul_ps(
				_mm256_mul_ps(
					_mm256_mul_ps(rhs, _mm256_set_ps(-0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f)), r),
				_mm256_mul_ps(r, r)));
	}

	__forceinline const SSEFloat Rsqrt(const SSEFloat& rhs)
	{
		const SSEFloat r = _mm_rsqrt_ps(rhs.m128);
		return _mm_add_ps(
			_mm_mul_ps(_mm_set_ps(1.5f, 1.5f, 1.5f, 1.5f), r),
			_mm_mul_ps(_mm_mul_ps(_mm_mul_ps(rhs, _mm_set_ps(-0.5f, -0.5f, -0.5f, -0.5f)), r),
			           _mm_mul_ps(r, r)));
	}

	__forceinline Vec<3, AVXFloat> Normalize(const Vec<3, AVXFloat>& v)
	{
		return v * Rsqrt(LengthSquared(v));
	}
}
