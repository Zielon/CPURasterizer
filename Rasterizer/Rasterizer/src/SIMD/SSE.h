#pragma once

#include "SSEBool.h"
#include "SSEInt.h"
#include "SSEFloat.h"

#include "../Config.h"

#include "../Math/Vec2.h"
#include "../Math/Vec3.h"

typedef Math::Vec<2, SSEFloat> SSEVec2f;
typedef Math::Vec<2, SSEInt> SSEVec2i;
typedef Math::Vec<2, SSEBool> SSEVec2b;

typedef Math::Vec<3, SSEFloat> SSEVec3f;
typedef Math::Vec<3, SSEInt> SSEVec3i;
typedef Math::Vec<3, SSEBool> SSEVec3b;

namespace SSE
{
	CFG_FORCE_INLINE bool All(const SSEBool& rhs) { return _mm_movemask_ps(rhs) == 0xf; }
	CFG_FORCE_INLINE bool Any(const SSEBool& rhs) { return _mm_movemask_ps(rhs) != 0x0; }
	CFG_FORCE_INLINE bool None(const SSEBool& rhs) { return _mm_movemask_ps(rhs) == 0x0; }

	CFG_FORCE_INLINE SSEFloat Select(const SSEBool& m, const SSEFloat& t, const SSEFloat& f)
	{
		return _mm_blendv_ps(f, t, m);
	}

	CFG_FORCE_INLINE SSEFloat operator +(const float& lhs, const SSEFloat& rhs) { return SSEFloat(lhs) + rhs; }
	CFG_FORCE_INLINE SSEFloat operator -(const float& lhs, const SSEFloat& rhs) { return SSEFloat(lhs) - rhs; }
	CFG_FORCE_INLINE SSEFloat operator *(const float& lhs, const SSEFloat& rhs) { return SSEFloat(lhs) * rhs; }
}
