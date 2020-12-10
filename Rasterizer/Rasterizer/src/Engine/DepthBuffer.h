#pragma once

#include "../SIMD/SSE.h"

namespace Engine
{
	class DepthBuffer final
	{
	public:
		SSEBool ZTest() const;
	};
}
