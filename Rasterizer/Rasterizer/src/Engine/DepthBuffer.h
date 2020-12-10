#pragma once

#include <array>
#include <deque>
#include <vector>

#include "../SIMD/SSE.h"

#include "Settings.h"

namespace Engine
{
	using TileDepth2D = std::array<std::array<SSEFloat, TILE_SIZE / 2>, TILE_SIZE / 2>;
	using TileDepth3D = std::array<TileDepth2D, 4>;

	class DepthBuffer final
	{
	public:
		DepthBuffer(const Settings& settings);

		void Clear();
		[[nodiscard]] SSEBool ZTest(const SSEFloat& depth, int x, int y, uint32_t sampleId, const SSEBool& mask);
	private:
		const Settings& settings;
		std::vector<TileDepth3D> buffer;
	};
}
