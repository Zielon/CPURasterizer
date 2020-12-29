#pragma once

#include <immintrin.h>

#include "../Config.h"

namespace AVX
{
	CFG_FORCE_INLINE __m256d _mm256_cmpeq_pd(__m256d a, __m256d b) { return _mm256_cmp_pd(a, b, _CMP_EQ_OQ); }
	CFG_FORCE_INLINE __m256d _mm256_cmpneq_pd(__m256d a, __m256d b) { return _mm256_cmp_pd(a, b, _CMP_NEQ_UQ); }
	CFG_FORCE_INLINE __m256d _mm256_cmplt_pd(__m256d a, __m256d b) { return _mm256_cmp_pd(a, b, _CMP_LT_OS); }
	CFG_FORCE_INLINE __m256d _mm256_cmpnlt_pd(__m256d a, __m256d b) { return _mm256_cmp_pd(a, b, _CMP_NLT_US); }
	CFG_FORCE_INLINE __m256d _mm256_cmple_pd(__m256d a, __m256d b) { return _mm256_cmp_pd(a, b, _CMP_LE_OS); }
	CFG_FORCE_INLINE __m256d _mm256_cmpnle_pd(__m256d a, __m256d b) { return _mm256_cmp_pd(a, b, _CMP_NLE_US); }
	CFG_FORCE_INLINE __m256d _mm256_cmpord_pd(__m256d a, __m256d b) { return _mm256_cmp_pd(a, b, _CMP_ORD_Q); }
	CFG_FORCE_INLINE __m256d _mm256_cmpunord_pd(__m256d a, __m256d b) { return _mm256_cmp_pd(a, b, _CMP_UNORD_Q); }

	CFG_FORCE_INLINE __m256 _mm256_cmpeq_ps(__m256 a, __m256 b) { return _mm256_cmp_ps(a, b, _CMP_EQ_OQ); }
	CFG_FORCE_INLINE __m256 _mm256_cmpneq_ps(__m256 a, __m256 b) { return _mm256_cmp_ps(a, b, _CMP_NEQ_UQ); }
	CFG_FORCE_INLINE __m256 _mm256_cmplt_ps(__m256 a, __m256 b) { return _mm256_cmp_ps(a, b, _CMP_LT_OS); }
	CFG_FORCE_INLINE __m256 _mm256_cmpnlt_ps(__m256 a, __m256 b) { return _mm256_cmp_ps(a, b, _CMP_NLT_US); }
	CFG_FORCE_INLINE __m256 _mm256_cmpge_ps(__m256 a, __m256 b) { return _mm256_cmp_ps(a, b, _CMP_NLT_US); }
	CFG_FORCE_INLINE __m256 _mm256_cmple_ps(__m256 a, __m256 b) { return _mm256_cmp_ps(a, b, _CMP_LE_OS); }
	CFG_FORCE_INLINE __m256 _mm256_cmpnle_ps(__m256 a, __m256 b) { return _mm256_cmp_ps(a, b, _CMP_NLE_US); }
	CFG_FORCE_INLINE __m256 _mm256_cmpgt_ps(__m256 a, __m256 b) { return _mm256_cmp_ps(a, b, _CMP_NLE_US); }
	CFG_FORCE_INLINE __m256 _mm256_cmpord_ps(__m256 a, __m256 b) { return _mm256_cmp_ps(a, b, _CMP_ORD_Q); }
	CFG_FORCE_INLINE __m256 _mm256_cmpunord_ps(__m256 a, __m256 b) { return _mm256_cmp_ps(a, b, _CMP_UNORD_Q); }
}
