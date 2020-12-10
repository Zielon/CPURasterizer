#include "DepthBuffer.h"

namespace Engine
{
	SSEBool DepthBuffer::ZTest() const
	{
		return SSEBool(true);
	}
}
