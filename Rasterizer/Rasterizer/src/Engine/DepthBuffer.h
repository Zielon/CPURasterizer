#pragma once

#include <array>
#include <deque>
#include <vector>

#include "../SIMD/AVX.h"

#include "Settings.h"

namespace Engine
{
	using TileDepth2D = std::array<std::array<AVXFloat, TILE_SIZE / 2>, TILE_SIZE / 2>;
	using TileDepth3D = std::array<TileDepth2D, 4>;

	class DepthBuffer final
	{
	public:
		DepthBuffer(const Settings& settings);

		void Clear();
		[[nodiscard]] AVXBool ZTest(const AVXFloat& depth, int x, int y, uint32_t sampleId, const AVXBool& mask);
	private:
		const Settings& settings;
		std::vector<TileDepth3D> buffer;
	};
}
