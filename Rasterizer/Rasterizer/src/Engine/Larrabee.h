#pragma once

#include <glm/glm.hpp>

#include "Triangle.h"
#include "Settings.h"
#include "Tile.h"

namespace Engine
{
	/**
	 * \brief Larrabee rasterizer
	 */
	class Larrabee final
	{
	public:
		/**
		 * \brief Assign triangles to tiles
		 * \param bin Current coreId/bin
		 * \param rasterTrianglesBuffer Previously clipped triangles
		 * \param tiles 
		 */
		static void AssignTriangles(
			uint32_t bin,
			std::deque<LarrabeeTriangle>& rasterTrianglesBuffer,
			std::vector<Tile>& tiles)
		{
			for (auto& T : rasterTrianglesBuffer)
			{
				uint32_t minX = std::max(0, std::min(T.v0.x, std::min(T.v1.x, T.v2.x)) >> TILE);
				uint32_t maxX = std::min(TILE_DIM_X - 1, std::max(T.v0.x, std::max(T.v1.x, T.v2.x)) >> TILE);
				uint32_t minY = std::max(0, std::min(T.v0.y, std::min(T.v1.y, T.v2.y)) >> TILE);
				uint32_t maxY = std::min(TILE_DIM_Y - 1, std::max(T.v0.y, std::max(T.v1.y, T.v2.y)) >> TILE);

				// One pixel projected triangle
				if (maxX - minX < 2 && maxY - minY < 2)
				{
					for (uint32_t y = minY; y <= maxY; y++)
						for (uint32_t x = minX; x <= maxX; x++)
							tiles[y * TILE_DIM_X + x].Add(bin, T.id);

					continue;
				}

				for (uint32_t y = minY; y <= maxY; ++y)
				{
					for (uint32_t x = minX; x <= maxX; ++x)
					{
						glm::ivec2 pixelBase = glm::ivec2(x, y);

						const glm::ivec2 rejCornerOffset0 = glm::ivec2(T.rejectCorner0 % 2, T.rejectCorner0 / 2);
						const glm::ivec2 rejCornerOffset1 = glm::ivec2(T.rejectCorner1 % 2, T.rejectCorner1 / 2);
						const glm::ivec2 rejCornerOffset2 = glm::ivec2(T.rejectCorner2 % 2, T.rejectCorner2 / 2);

						const glm::ivec2 rejCorner0 = (pixelBase + rejCornerOffset0) << TILE;
						const glm::ivec2 rejCorner1 = (pixelBase + rejCornerOffset1) << TILE;
						const glm::ivec2 rejCorner2 = (pixelBase + rejCornerOffset2) << TILE;

						if (T.EdgeFunc0(rejCorner0) < 0 || T.EdgeFunc1(rejCorner1) < 0 || T.EdgeFunc2(rejCorner2) < 0)
							continue;

						const glm::ivec2 acptCornerOffset0 = glm::ivec2(T.acceptCorner0 % 2, T.acceptCorner0 / 2);
						const glm::ivec2 acptCornerOffset1 = glm::ivec2(T.acceptCorner1 % 2, T.acceptCorner1 / 2);
						const glm::ivec2 acptCornerOffset2 = glm::ivec2(T.acceptCorner2 % 2, T.acceptCorner2 / 2);

						const glm::ivec2 acptCorner0 = (pixelBase + acptCornerOffset0) << TILE;
						const glm::ivec2 acptCorner1 = (pixelBase + acptCornerOffset1) << TILE;
						const glm::ivec2 acptCorner2 = (pixelBase + acptCornerOffset2) << TILE;

						T.isTrivial =
							T.EdgeFunc0(acptCorner0) >= 0 &&
							T.EdgeFunc1(acptCorner1) >= 0 &&
							T.EdgeFunc2(acptCorner2) >= 0;

						tiles[y * TILE_DIM_X + x].Add(bin, T.id);
					}
				}
			}
		}
	};
}