#pragma once

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
		LarrabeeRasterizer(Ref<Buffer2D<LarrabeeTriangle>> trianglesBuffer, Ref<std::vector<Tile>> tiles):
			rasterTrianglesBuffer(trianglesBuffer), tiles(tiles) {}

		~LarrabeeRasterizer();

		void RasterizeTile(uint32_t bin, Tile& tile) const;

		void AssignTriangles(uint32_t bin) const;

	private:
		const Ref<Buffer2D<LarrabeeTriangle>> rasterTrianglesBuffer;
		const Ref<std::vector<Tile>> tiles;
	};
}
