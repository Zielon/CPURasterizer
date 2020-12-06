#pragma once

#include <deque>
#include <vector>

#include <glm/glm.hpp>

namespace Engine
{
	/**
	 * \brief Lock free Tile structure
	 */
	struct Tile
	{
		Tile() = default;

		Tile(const glm::ivec2& min, const glm::ivec2 max, uint32_t id): maxRaster(max), minRaster(min), id(id)
		{
			bins.resize(16);
		}

		glm::ivec2 maxRaster{}; // Bottom right tile's corner
		glm::ivec2 minRaster{}; // Upper left corner
		uint32_t id{};

		void Add(uint32_t bin, uint32_t elem)
		{
			bins[bin][binsIndex[bin]] = elem;
			++binsIndex[bin];
		};

		void Clear()
		{
			std::fill(binsIndex.begin(), binsIndex.end(), 0);
		}

	private:
		std::array<uint32_t, 16> binsIndex;
		std::vector<std::array<uint32_t, 1024>> bins;
	};
}
