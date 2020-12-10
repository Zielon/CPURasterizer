#include "LarrabeeRasterizer.h"

#include <array>
#include <glm/glm.hpp>

#include "../SIMD/AVX.h"
#include "../SIMD/SSE.h"

#include "Settings.h"
#include "Triangle.h"
#include "Tile.h"
#include "DepthBuffer.h"

namespace Engine
{
	LarrabeeRasterizer::~LarrabeeRasterizer() {};

	void LarrabeeRasterizer::RasterizeTile(uint32_t bin, Tile& tile) const
	{
		SSEVec2i centerOffset(SSEInt(8, 24, 8, 24), SSEInt(8, 8, 24, 24));

		for (int i = 0; i < tile.binsIndex[bin]; ++i)
		{
			const uint32_t id = tile.trinagles[bin][i];

			LarrabeeTriangle triangle = rasterTrianglesBuffer[bin][id];

			int minX = std::max(tile.minRaster.x,
			                    std::min(triangle.v0.x, std::min(triangle.v1.x, triangle.v2.x)) >> FIXED_POINT);
			int maxX = std::min(tile.maxRaster.x - 1,
			                    std::max(triangle.v0.x, std::max(triangle.v1.x, triangle.v2.x)) >> FIXED_POINT);
			int minY = std::max(tile.minRaster.y,
			                    std::min(triangle.v0.y, std::min(triangle.v1.y, triangle.v2.y)) >> FIXED_POINT);
			int maxY = std::min(tile.maxRaster.y - 1,
			                    std::max(triangle.v0.y, std::max(triangle.v1.y, triangle.v2.y)) >> FIXED_POINT);
			minX -= minX % 2;
			minY -= minY % 2;

			if (maxX < minX || maxY < minY)
				return;

			SSELarrabeeTriangle SSEtriangle(rasterTrianglesBuffer[bin][id]);

			SSEVec2i pixelBase(minX << FIXED_POINT, minY << FIXED_POINT);
			SSEVec2i pixelCenter = pixelBase + centerOffset;
			SSEInt edgeVal0 = SSEtriangle.EdgeFunc0(pixelCenter);
			SSEInt edgeVal1 = SSEtriangle.EdgeFunc1(pixelCenter);
			SSEInt edgeVal2 = SSEtriangle.EdgeFunc2(pixelCenter);

			glm::ivec2 pixelCrd;
			SSEInt pixelBaseStep = SSEInt(32);
			for (pixelCrd.y = minY; pixelCrd.y <= maxY; pixelCrd.y += 2, pixelBase.y += pixelBaseStep)
			{
				SSEInt edgeYBase0 = edgeVal0;
				SSEInt edgeYBase1 = edgeVal1;
				SSEInt edgeYBase2 = edgeVal2;

				pixelBase.x = SSEInt(minX << 4);
				for (pixelCrd.x = minX; pixelCrd.x <= maxX; pixelCrd.x += 2, pixelBase.x += pixelBaseStep)
				{
					pixelCenter = pixelBase + centerOffset;
					SSEBool covered = (edgeVal0 | edgeVal1 | edgeVal2) >= SSEInt(0);

					if (Any(covered))
					{
						SSEtriangle.CalcBarycentricCoord(pixelCenter.x, pixelCenter.y);

						auto tbin = SSEtriangle.binId;
						auto& ids = SSEtriangle.vertexIds;

						float v0 = clippedProjectedVertexBuffer[tbin][ids[0]].projectedPosition.z;
						float v1 = clippedProjectedVertexBuffer[tbin][ids[1]].projectedPosition.z;
						float v2 = clippedProjectedVertexBuffer[tbin][ids[2]].projectedPosition.z;

						SSEBool ztest =
							depthBuffer.ZTest(SSEtriangle.GetDepth(v0, v1, v2), pixelCrd.x, pixelCrd.y, 0, covered);

						SSEBool visible = ztest & covered;
						if (Any(visible))
						{
							tile.fragments.push_back(Pixel());
						}
					}

					edgeVal0 += SSEtriangle.deltaY0;
					edgeVal1 += SSEtriangle.deltaY1;
					edgeVal2 += SSEtriangle.deltaY2;
				}

				edgeVal0 = edgeYBase0 + SSEtriangle.deltaX0;
				edgeVal1 = edgeYBase1 + SSEtriangle.deltaX1;
				edgeVal2 = edgeYBase2 + SSEtriangle.deltaX2;
			}
		}
	}

	void LarrabeeRasterizer::AssignTriangles(uint32_t bin) const
	{
		const int shift = TILE + FIXED_POINT;

		for (auto& T : rasterTrianglesBuffer[bin])
		{
			uint32_t minX = std::max(0, std::min(T.v0.x, std::min(T.v1.x, T.v2.x)) >> shift);
			uint32_t maxX = std::min(TILE_DIM_X - 1, std::max(T.v0.x, std::max(T.v1.x, T.v2.x)) >> shift);
			uint32_t minY = std::max(0, std::min(T.v0.y, std::min(T.v1.y, T.v2.y)) >> shift);
			uint32_t maxY = std::min(TILE_DIM_Y - 1, std::max(T.v0.y, std::max(T.v1.y, T.v2.y)) >> shift);

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

					const glm::ivec2 rejCorner0 = (pixelBase + rejCornerOffset0) << shift;
					const glm::ivec2 rejCorner1 = (pixelBase + rejCornerOffset1) << shift;
					const glm::ivec2 rejCorner2 = (pixelBase + rejCornerOffset2) << shift;

					if (T.EdgeFunc0(rejCorner0) < 0 || T.EdgeFunc1(rejCorner1) < 0 || T.EdgeFunc2(rejCorner2) < 0)
						continue;

					const glm::ivec2 acptCornerOffset0 = glm::ivec2(T.acceptCorner0 % 2, T.acceptCorner0 / 2);
					const glm::ivec2 acptCornerOffset1 = glm::ivec2(T.acceptCorner1 % 2, T.acceptCorner1 / 2);
					const glm::ivec2 acptCornerOffset2 = glm::ivec2(T.acceptCorner2 % 2, T.acceptCorner2 / 2);

					const glm::ivec2 acptCorner0 = (pixelBase + acptCornerOffset0) << shift;
					const glm::ivec2 acptCorner1 = (pixelBase + acptCornerOffset1) << shift;
					const glm::ivec2 acptCorner2 = (pixelBase + acptCornerOffset2) << shift;

					T.isTrivial =
						T.EdgeFunc0(acptCorner0) >= 0 &&
						T.EdgeFunc1(acptCorner1) >= 0 &&
						T.EdgeFunc2(acptCorner2) >= 0;

					tiles[y * TILE_DIM_X + x].Add(bin, T.id);
				}
			}
		}
	}
}
