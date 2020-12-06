#pragma once

#include <deque>
#include <glm/glm.hpp>

namespace Engine
{
	class Tile
	{
	public:
		Tile(const glm::ivec2& max, const glm::ivec2 min, uint32_t id): maxRaster(max), minRaster(min), id(id) {}

		glm::ivec2 maxRaster; // Bottom right tile's corner
		glm::ivec2 minRaster; // Upper left corner
		uint32_t id;

		std::deque<> triangles;
		
	};
}
