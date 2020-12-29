#pragma once

////////////////////////////////////////////////////////////////////////////////
// Compiler detection (based on GLM's compiler detection, under MIT license)
////////////////////////////////////////////////////////////////////////////////

#define CFG_COMPILER_UNKNOWN  0x00000000
#define CFG_COMPILER_VC       0x01000000
#define CFG_COMPILER_GCC      0x02000000
#define CFG_COMPILER_CLANG    0x20000000

#if defined(__clang__)
#define CFG_COMPILER CFG_COMPILER_CLANG
#elif defined(_MSC_VER)
#define CFG_COMPILER CFG_COMPILER_VC
#elif defined(__GNUC__) || defined(__MINGW32__)
#define CFG_COMPILER CFG_COMPILER_GCC
#else
#define CFG_COMPILER CFG_COMPILER_UNKNOWN
#endif


////////////////////////////////////////////////////////////////////////////////
// Useful macros
////////////////////////////////////////////////////////////////////////////////

// Force inlining
#if (CFG_COMPILER & CFG_COMPILER_VC)
#  define CFG_FORCE_INLINE __forceinline
#elif (CFG_COMPILER & (CFG_COMPILER_GCC | CFG_COMPILER_CLANG))
#  define CFG_FORCE_INLINE inline __attribute__((__always_inline__))
#else
#  define CFG_FORCE_INLINE inline
#endif
