#include "DepthBuffer.h"

#include "Concurrency.h"

namespace Engine
{
	DepthBuffer::DepthBuffer(const Settings& settings): settings(settings)
	{
		buffer.resize(TILE_DIM_X * TILE_DIM_Y);
		Clear();
	}

	void DepthBuffer::Clear()
	{
		Concurrency::ForEach(buffer.begin(), buffer.end(), [this](TileDepth3D& tile3D)
		{
			for (int i = 0; i < 4; ++i)
				for (auto& tile2D : tile3D[i])
					for (auto& tile1D : tile2D)
						tile1D = INFINITY;
		});
	}

	SSEBool DepthBuffer::ZTest(const SSEFloat& depth, int x, int y, uint32_t sampleId, const SSEBool& mask)
	{
		const int tileX = x >> TILE;
		const int tileY = y >> TILE;

		TileDepth3D& tile3D = buffer[tileY * TILE_DIM_X + tileX];

		const int intraTileX = x & (TILE_SIZE - 1);
		const int intraTileY = y & (TILE_SIZE - 1);

		SSEFloat& currentDepth = tile3D[sampleId][intraTileX >> 1][(TILE_SIZE - 1 - intraTileY) >> 1];

		SSEBool ret = depth <= currentDepth;
		currentDepth = SSE::Select(ret & mask, depth, currentDepth);

		return ret;
	}
}
