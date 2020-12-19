#include "Rasterizer.h"

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
	Rasterizer::~Rasterizer() = default;;

	void Rasterizer::RasterizeTile(uint32_t bin, Tile& tile) const
	{
		for (int i = 0; i < tile.binsIndex[bin]; ++i)
		{
			const uint32_t id = tile.triangles[bin][i];

			LarrabeeTriangle& triangle = rasterTrianglesBuffer[bin][id];

			int minX = std::max(tile.minRaster.x,
			                    std::min(triangle.v0.x, std::min(triangle.v1.x, triangle.v2.x)) >> FIXED_POINT);
			int maxX = std::min(tile.maxRaster.x - 1,
			                    std::max(triangle.v0.x, std::max(triangle.v1.x, triangle.v2.x)) >> FIXED_POINT);
			int minY = std::max(tile.minRaster.y,
			                    std::min(triangle.v0.y, std::min(triangle.v1.y, triangle.v2.y)) >> FIXED_POINT);
			int maxY = std::min(tile.maxRaster.y - 1,
			                    std::max(triangle.v0.y, std::max(triangle.v1.y, triangle.v2.y)) >> FIXED_POINT);
			minX -= minX % 4;
			minY -= minY % 2;

			if (maxX < minX || maxY < minY)
				return;

			AVXLarrabeeTriangle AVXtriangle(triangle);

			AVXVec2i pixelBase(minX << FIXED_POINT, minY << FIXED_POINT);
			AVXVec2i pixelCenter = pixelBase + centerOffset;
			AVXInt edgeVal0 = AVXtriangle.EdgeFunc0(pixelCenter);
			AVXInt edgeVal1 = AVXtriangle.EdgeFunc1(pixelCenter);
			AVXInt edgeVal2 = AVXtriangle.EdgeFunc2(pixelCenter);

			AVXInt pixelBaseStepY = AVXInt(32);
			AVXInt pixelBaseStepX = AVXInt(64);
			AVXInt zero = AVXInt(0);

			for (int y = minY; y <= maxY; y += 2, pixelBase.y += pixelBaseStepY)
			{
				AVXInt edgeYBase0 = edgeVal0;
				AVXInt edgeYBase1 = edgeVal1;
				AVXInt edgeYBase2 = edgeVal2;

				pixelBase.x = AVXInt(minX << FIXED_POINT);
				for (int x = minX; x <= maxX; x += 4, pixelBase.x += pixelBaseStepX)
				{
					pixelCenter = pixelBase + centerOffset;
					auto sum = (edgeVal0 | edgeVal1 | edgeVal2);
					AVXBool covered = sum >= zero;

					if (AVX::Any(covered))
					{
						AVXtriangle.CalcBarycentricCoord(pixelCenter.x, pixelCenter.y);

						auto tbin = AVXtriangle.binId;
						auto& ids = AVXtriangle.vertexIds;

						float v0 = clippedProjectedVertexBuffer[tbin][ids[0]].projectedPosition.z;
						float v1 = clippedProjectedVertexBuffer[tbin][ids[1]].projectedPosition.z;
						float v2 = clippedProjectedVertexBuffer[tbin][ids[2]].projectedPosition.z;

						AVXBool ztest = depthBuffer.ZTest(AVXtriangle.GetDepth(v0, v1, v2), x, y, 0, covered);

						AVXBool visible = ztest & covered;

						if (AVX::Any(visible))
						{
							tile.pixels.emplace_back(
								AVXtriangle.lambda0,
								AVXtriangle.lambda1,
								ids[0], ids[1], ids[2],
								tbin,
								AVXtriangle.materialId,
								glm::ivec2(x, y),
								CoverageMask(visible),
								tile.id,
								tile.pixels.size());
						}
					}

					edgeVal0 += AVXtriangle.deltaY0;
					edgeVal1 += AVXtriangle.deltaY1;
					edgeVal2 += AVXtriangle.deltaY2;
				}

				edgeVal0 = edgeYBase0 + AVXtriangle.deltaX0;
				edgeVal1 = edgeYBase1 + AVXtriangle.deltaX1;
				edgeVal2 = edgeYBase2 + AVXtriangle.deltaX2;
			}
		}
	}

	void Rasterizer::AssignTriangles(uint32_t bin) const
	{
		const int shift = TILE + FIXED_POINT;

		for (auto& T : rasterTrianglesBuffer[bin])
		{
			uint32_t minX = std::max(0, std::min(T.v0.x, std::min(T.v1.x, T.v2.x)) >> shift);
			uint32_t maxX = std::min(tileDimX - 1, std::max(T.v0.x, std::max(T.v1.x, T.v2.x)) >> shift);
			uint32_t minY = std::max(0, std::min(T.v0.y, std::min(T.v1.y, T.v2.y)) >> shift);
			uint32_t maxY = std::min(tileDimY - 1, std::max(T.v0.y, std::max(T.v1.y, T.v2.y)) >> shift);

			// One pixel projected triangle
			if (maxX - minX < 2 && maxY - minY < 2)
			{
				for (uint32_t y = minY; y <= maxY; y++)
					for (uint32_t x = minX; x <= maxX; x++)
						tiles[y * tileDimX + x].Add(bin, T.id);

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

					tiles[y * tileDimX + x].Add(bin, T.id);
				}
			}
		}
	}
}
