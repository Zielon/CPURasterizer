#pragma once

#include <cstdint>
#include <array>
#include <glm/glm.hpp>

#include "Settings.h"

#include "../SIMD/AVX.h"

namespace Engine
{
	struct Triangle
	{
		Triangle() = default;

		Triangle(
			const glm::vec2& v0,
			const glm::vec2& v1,
			const glm::vec2& v2,
			uint32_t id,
			uint32_t binId,
			uint32_t materialId,
			std::array<uint32_t, 3> ids):
			v0(v0), v1(v1), v2(v2), id(id), binId(binId), materialId(materialId), vertexIds(ids) { }

		glm::ivec2 v0{};
		glm::ivec2 v1{};
		glm::ivec2 v2{};
		uint32_t id{};
		uint32_t binId{};
		uint32_t materialId{};
		std::array<uint32_t, 3> vertexIds;
	};

	/**
	 * \brief Based on:
	 * http://www.cs.cmu.edu/afs/cs/academic/class/15869-f11/www/readings/abrash09_lrbrast.pdf
	 * https://github.com/behindthepixels/EDXRaster/blob/master/EDXRaster/Core/RasterTriangle.h
	 */
	struct LarrabeeTriangle final : Triangle
	{
	public:
		int B0{}, C0{}, B1{}, C1{}, B2{}, C2{};
		int deltaY0, deltaX0, deltaY1, deltaX1, detalY2, deltaX2;
		float invDet{};
		// A tile trivial reject test.
		uint32_t rejectCorner0{}, rejectCorner1{}, rejectCorner2{};
		// The tile trivial accept test.
		uint32_t acceptCorner0{}, acceptCorner1{}, acceptCorner2{};
		// Barycentrinc coordinates
		float lambda0{}, lambda1{};
		// Triangle completely inside a tile
		bool isTrivial{};
		float fixed;

		LarrabeeTriangle(
			const glm::vec2& v0,
			const glm::vec2& v1,
			const glm::vec2& v2,
			uint32_t id,
			uint32_t binId,
			uint32_t materialId,
			std::array<uint32_t, 3> ids): Triangle(v0, v1, v2, id, binId, materialId, ids)
		{
			fixed = 16.f;

			this->v0 = v0 * fixed;
			this->v1 = v1 * fixed;
			this->v2 = v2 * fixed;
		}

		[[nodiscard]] __forceinline int TopLeftEdge(const glm::ivec2& v1, const glm::ivec2& v2) const
		{
			return ((v2.y > v1.y) || (v1.y == v2.y && v1.x > v2.x)) ? 0 : -1;
		}

		[[nodiscard]] __forceinline int EdgeFunc0(const glm::ivec2& p) const
		{
			return B0 * (p.x - v0.x) + C0 * (p.y - v0.y) + TopLeftEdge(v0, v1);
		}

		[[nodiscard]] __forceinline int EdgeFunc1(const glm::ivec2& p) const
		{
			return B1 * (p.x - v1.x) + C1 * (p.y - v1.y) + TopLeftEdge(v1, v2);
		}

		[[nodiscard]] __forceinline int EdgeFunc2(const glm::ivec2& p) const
		{
			return B2 * (p.x - v2.x) + C2 * (p.y - v2.y) + TopLeftEdge(v2, v0);
		}

		[[nodiscard]] __forceinline bool Inside(const glm::ivec2& p) const
		{
			return (EdgeFunc0(p) | EdgeFunc1(p) | EdgeFunc2(p)) >= 0;
		}

		[[nodiscard]] __forceinline float GetDepth(float z0, float z1, float z2) const
		{
			const float lambda2 = 1.0f - lambda0 - lambda1;
			return lambda0 * z0 + lambda1 * z1 + lambda2 * z2;
		}

		__forceinline void CalcBarycentricCoord(const int x, const int y)
		{
			lambda0 = (B1 * (x - v2.x) + C1 * (y - v2.y)) * invDet;
			lambda1 = (B2 * (x - v2.x) + C2 * (y - v2.y)) * invDet;
		}

		/**
		 * \brief Larrabee rasterizers performs set of tests to define with which tiles a given triangle intersects.
		 * This methods sets values for accepted and rejected tiles.
		 * \return True is setup went correctly
		 */
		bool Setup(bool cullBackFaces)
		{
			// Calculate edges
			// Edge winding is important
			B0 = v0.y - v1.y;
			C0 = v1.x - v0.x;
			B1 = v1.y - v2.y;
			C1 = v2.x - v1.x;
			B2 = v2.y - v0.y;
			C2 = v0.x - v2.x;

			const int det = C2 * B1 - C1 * B2;

			if (cullBackFaces)
			{
				if (det <= 0)
					return false;
			}
			else
			{
				if (det == 0)
					return false;

				// Allow back-faces
				if (det < 0)
				{
					B0 *= -1;
					B1 *= -1;
					B2 *= -1;
					C0 *= -1;
					C1 *= -1;
					C2 *= -1;
				}
			}

			deltaY0 = 16 * B0;
			deltaX0 = 16 * C0;
			deltaY1 = 16 * B1;
			deltaX1 = 16 * C1;
			detalY2 = 16 * B2;
			deltaX2 = 16 * C2;

			invDet = 1.0f / static_cast<float>(std::abs(det));

			// Determining which corner is the trivial reject corner will vary from edge to edge depending on slope
			const float edge0Slope = B0 / static_cast<float>(C0);
			const float edge1Slope = B1 / static_cast<float>(C1);
			const float edge2Slope = B2 / static_cast<float>(C2);

			// For edge0
			if (edge0Slope >= 0.0f)
			{
				if (C0 >= 0.0f)
				{
					rejectCorner0 = 3;
					acceptCorner0 = 0;
				}
				else
				{
					rejectCorner0 = 0;
					acceptCorner0 = 3;
				}
			}
			else
			{
				if (C0 >= 0.0f)
				{
					rejectCorner0 = 2;
					acceptCorner0 = 1;
				}
				else
				{
					rejectCorner0 = 1;
					acceptCorner0 = 2;
				}
			}
			// For edge1
			if (edge1Slope >= 0.0f)
			{
				if (C1 >= 0.0f)
				{
					rejectCorner1 = 3;
					acceptCorner1 = 0;
				}
				else
				{
					rejectCorner1 = 0;
					acceptCorner1 = 3;
				}
			}
			else
			{
				if (C1 >= 0.0f)
				{
					rejectCorner1 = 2;
					acceptCorner1 = 1;
				}
				else
				{
					rejectCorner1 = 1;
					acceptCorner1 = 2;
				}
			}
			// For edge2
			if (edge2Slope >= 0.0f)
			{
				if (C2 >= 0.0f)
				{
					rejectCorner2 = 3;
					acceptCorner2 = 0;
				}
				else
				{
					rejectCorner2 = 0;
					acceptCorner2 = 3;
				}
			}
			else
			{
				if (C2 >= 0.0f)
				{
					rejectCorner2 = 2;
					acceptCorner2 = 1;
				}
				else
				{
					rejectCorner2 = 1;
					acceptCorner2 = 2;
				}
			}

			return true;
		};
	};

	struct AVXLarrabeeTriangle
	{
		AVXVec2i v0, v1, v2;
		AVXInt B0, C0, B1, C1, B2, C2;
		AVXInt deltaY0, deltaX0, deltaY1, deltaX1, deltaY2, deltaX2;
		AVXFloat lambda0, lambda1;
		AVXFloat invDet;
		uint32_t id{};
		uint32_t binId{};
		uint32_t materialId{};
		std::array<uint32_t, 3> vertexIds;

		AVXLarrabeeTriangle(const LarrabeeTriangle& triangle) :
			v0(triangle.v0)
			, v1(triangle.v1)
			, v2(triangle.v2)
			, B0(triangle.B0)
			, C0(triangle.C0)
			, B1(triangle.B1)
			, C1(triangle.C1)
			, B2(triangle.B2)
			, C2(triangle.C2)
			, deltaY0(4 * triangle.deltaY0)
			, deltaX0(2 * triangle.deltaX0)
			, deltaY1(4 * triangle.deltaY1)
			, deltaX1(2 * triangle.deltaX1)
			, deltaY2(4 * triangle.detalY2)
			, deltaX2(2 * triangle.deltaX2)
			, invDet(triangle.invDet)
			, binId(triangle.binId)
			, materialId(triangle.materialId)
			, vertexIds(triangle.vertexIds) { }

		[[nodiscard]] __forceinline AVXInt TopLeftEdge(const AVXVec2i& v1, const AVXVec2i& v2) const
		{
			AVXBool r = ((v2.y > v1.y) | ((v1.y == v2.y) & (v1.x > v2.x)));
			return AVXInt(r);
		}

		[[nodiscard]] __forceinline AVXInt EdgeFunc0(const AVXVec2i& p) const
		{
			return B0 * (p.x - v0.x) + C0 * (p.y - v0.y) + TopLeftEdge(v0, v1);
		}

		[[nodiscard]] __forceinline AVXInt EdgeFunc1(const AVXVec2i& p) const
		{
			return B1 * (p.x - v1.x) + C1 * (p.y - v1.y) + TopLeftEdge(v1, v2);
		}

		[[nodiscard]] __forceinline AVXInt EdgeFunc2(const AVXVec2i& p) const
		{
			return B2 * (p.x - v2.x) + C2 * (p.y - v2.y) + TopLeftEdge(v2, v0);
		}

		[[nodiscard]] __forceinline AVXBool Inside(const AVXVec2i& p) const
		{
			return (EdgeFunc0(p) | EdgeFunc1(p) | EdgeFunc2(p)) >= AVXInt(0);
		}

		[[nodiscard]] __forceinline bool TrivialReject(const AVXVec2i& p) const
		{
			return AVX::Any((EdgeFunc0(p) & EdgeFunc1(p) & EdgeFunc2(p)) < AVXInt(0));
		}

		[[nodiscard]] __forceinline AVXFloat GetDepth(float z0, float z1, float z2) const
		{
			const auto One = AVXFloat(1);
			const AVXFloat lambda2 = One - lambda0 - lambda1;
			return lambda0 * z0 + lambda1 * z1 + lambda2 * z2;
		}

		__forceinline void CalcBarycentricCoord(const AVXInt& x, const AVXInt& y)
		{
			lambda0 = AVXFloat((B1 * (x - v2.x) + C1 * (y - v2.y))) * invDet;
			lambda1 = AVXFloat((B2 * (x - v2.x) + C2 * (y - v2.y))) * invDet;
		}
	};
}
