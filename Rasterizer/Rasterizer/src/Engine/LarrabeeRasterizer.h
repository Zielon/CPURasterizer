#pragma once

#include "DepthBuffer.h"
#include "Triangle.h"
#include "Tile.h"

namespace Engine
{
	template <class T>
	using Buffer2D = std::vector<std::deque<T>>;

	template <class T>
	using Ref = std::reference_wrapper<T>;

	/**
	 * \brief Larrabee rasterizer
	 */
	class LarrabeeRasterizer final
	{
	public:
		LarrabeeRasterizer(
			Buffer2D<LarrabeeTriangle>& trianglesBuffer,
			std::vector<Tile>& tiles,
			DepthBuffer& depthBuffer):
			rasterTrianglesBuffer(trianglesBuffer), tiles(tiles), depthBuffer(depthBuffer) {}

		~LarrabeeRasterizer();

		void RasterizeTile(uint32_t bin, Tile& tile) const;

		void AssignTriangles(uint32_t bin) const;

	private:
		Buffer2D<LarrabeeTriangle>& rasterTrianglesBuffer;
		std::vector<Tile>& tiles;
		class DepthBuffer& depthBuffer;
	};
}
