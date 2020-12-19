#pragma once

#include "DepthBuffer.h"
#include "Triangle.h"
#include "Tile.h"
#include "../Assets/Vertex.h"

namespace Engine
{
	template <class T>
	using Buffer2D = std::vector<std::vector<T>>;

	template <class T>
	using Ref = std::reference_wrapper<T>;

	/**
	 * \brief Larrabee rasterizer
	 */
	class Rasterizer final
	{
	public:
		Rasterizer(
			int width,
			int height,
			Buffer2D<LarrabeeTriangle>& trianglesBuffer,
			std::vector<Tile>& tiles,
			Buffer2D<Assets::Vertex>& clippedProjectedVertexBuffer,
			DepthBuffer& depthBuffer):
			tileDimX((width + TILE_SIZE - 1) >> TILE),
			tileDimY((height + TILE_SIZE - 1) >> TILE),
			rasterTrianglesBuffer(trianglesBuffer),
			clippedProjectedVertexBuffer(clippedProjectedVertexBuffer), tiles(tiles),
			depthBuffer(depthBuffer)
		{
			centerOffset = AVXVec2i(AVXInt(8, 24, 8, 24, 40, 56, 40, 56), AVXInt(8, 8, 24, 24, 8, 8, 24, 24));
		}

		~Rasterizer();

		void RasterizeTile(uint32_t bin, Tile& tile) const;

		void AssignTriangles(uint32_t bin) const;

	private:
		int tileDimX;
		int tileDimY;
		Buffer2D<LarrabeeTriangle>& rasterTrianglesBuffer;
		Buffer2D<Assets::Vertex>& clippedProjectedVertexBuffer;
		std::vector<Tile>& tiles;
		class DepthBuffer& depthBuffer;
		AVXVec2i centerOffset;;
	};
}
