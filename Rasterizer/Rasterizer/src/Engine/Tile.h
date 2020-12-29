#pragma once

#include <glm/glm.hpp>

#include <thread>
#include <vector>

#include "Concurrency.h"
#include "Pixel.h"

namespace Engine
{
	constexpr int BIN_SIZE = 4096;

	/**
	 * \brief Lock free Tile structure
	 */
	struct Tile
	{
		Tile() = default;

		Tile(const glm::ivec2& min, const glm::ivec2 max, uint32_t id): id(id), maxRaster(max), minRaster(min)
		{
			bins = std::thread::hardware_concurrency();
			pixels.reserve(256);
			triangles.resize(bins);
			binsIndex.resize(bins);
			for (auto& triangle : triangles)
				triangle.resize(BIN_SIZE);
		}

		uint32_t id{};
		uint32_t bins{};
		glm::ivec2 maxRaster{}; // Bottom right tile's corner
		glm::ivec2 minRaster{}; // Upper left tile's corner
		std::vector<uint32_t> binsIndex{};
		std::vector<std::vector<uint32_t>> triangles;
		std::vector<Pixel> pixels;

		/**
		 * \brief Lock free primitive addition method
		 * \param bin Bin id
		 * \param elem 
		 */
		void Add(uint32_t bin, uint32_t elem)
		{
			if (binsIndex[bin] > BIN_SIZE - 1)
				triangles[bin].push_back(elem);
			else
				triangles[bin][binsIndex[bin]] = elem;

			++binsIndex[bin];
		};

		void Clear()
		{
			pixels.clear();
			std::fill(binsIndex.begin(), binsIndex.end(), 0);
			Concurrency::ForEach(triangles.begin(), triangles.end(),
			                     [&](auto& triangle) { triangle.resize(BIN_SIZE); });
		}
	};
}
