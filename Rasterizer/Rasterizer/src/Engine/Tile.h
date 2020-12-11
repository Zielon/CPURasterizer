#pragma once

#include <glm/glm.hpp>

#include <deque>
#include <vector>
#include <array>

#include "Pixel.h"
#include "../Assets/Color4b.h"

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
			trinagles.resize(16);
			fragments.reserve(100);
			color = { uint8_t(rand() & 0xff), uint8_t(rand() & 0xff), uint8_t(rand() & 0xff) };
		}

		glm::ivec2 maxRaster{}; // Bottom right tile's corner
		glm::ivec2 minRaster{}; // Upper left tile's corner
		uint32_t id{};
		Assets::Color4b color;

		/**
		 * \brief Lock free primitive addition method
		 * \param bin Bin id
		 * \param elem 
		 */
		void Add(uint32_t bin, uint32_t elem)
		{
			trinagles[bin][binsIndex[bin]] = elem;
			++binsIndex[bin];
		};

		void Clear()
		{
			std::fill(binsIndex.begin(), binsIndex.end(), 0);
			fragments.clear();
		}

		std::array<uint32_t, 16> binsIndex{};
		std::vector<std::array<uint32_t, 4096>> trinagles;
		std::vector<Pixel> fragments;
	};
}
