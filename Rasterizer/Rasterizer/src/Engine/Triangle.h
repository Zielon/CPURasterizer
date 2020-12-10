#pragma once

#include <cstdint>
#include <array>
#include <glm/glm.hpp>

#include "../SIMD/SSE.h"

using namespace SSE;

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
			uint32_t textureId,
			std::array<uint32_t, 3> ids):
			v0(v0), v1(v1), v2(v2), id(id), binId(binId), textureId(textureId), vertexIds(ids) { }

		glm::ivec2 v0{};
		glm::ivec2 v1{};
		glm::ivec2 v2{};
		uint32_t id{};
		uint32_t binId{};
		uint32_t textureId{};
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
		int stepB0, stepC0, stepB1, stepC1, stepB2, stepC2;
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
			uint32_t textureId,
			std::array<uint32_t, 3> ids): Triangle(v0, v1, v2, id, binId, textureId, ids)
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
		bool Setup()
		{
			// Calculate edges
			B0 = v0.y - v1.y;
			C0 = v1.x - v0.x;
			B1 = v1.y - v2.y;
			C1 = v2.x - v1.x;
			B2 = v2.y - v0.y;
			C2 = v0.x - v2.x;

			const int det = C2 * B1 - C1 * B2;

			if (det <= 0)
				return false;

			invDet = 1.0f / static_cast<float>(det);

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

	struct SSELarrabeeTriangle
	{
		SSEVec2i v0, v1, v2;
		SSEInt B0, C0, B1, C1, B2, C2;
		SSEInt stepB0, stepC0, stepB1, stepC1, stepB2, stepC2;
		SSEFloat lambda0, lambda1;
		SSEFloat invDet;
		uint32_t id{};
		uint32_t binId{};
		uint32_t textureId{};
		std::array<uint32_t, 3> vertexIds;

		SSELarrabeeTriangle(const LarrabeeTriangle& triangle) :
			v0(triangle.v0)
			, v1(triangle.v1)
			, v2(triangle.v2)
			, B0(triangle.B0)
			, C0(triangle.C0)
			, B1(triangle.B1)
			, C1(triangle.C1)
			, B2(triangle.B2)
			, C2(triangle.C2)
			, stepB0(2 * triangle.stepB0)
			, stepC0(2 * triangle.stepC0)
			, stepB1(2 * triangle.stepB1)
			, stepC1(2 * triangle.stepC1)
			, stepB2(2 * triangle.stepB2)
			, stepC2(2 * triangle.stepC2)
			, invDet(triangle.invDet)
			, binId(triangle.binId)
			, textureId(triangle.textureId)
			, vertexIds(triangle.vertexIds) { }

		[[nodiscard]] __forceinline SSEInt TopLeftEdge(const SSEVec2i& v1, const SSEVec2i& v2) const
		{
			return SSEInt(((v2.y > v1.y) | ((v1.y == v2.y) & (v1.x > v2.x))).m128);
		}

		[[nodiscard]] __forceinline SSEInt EdgeFunc0(const SSEVec2i& p) const
		{
			return B0 * (p.x - v0.x) + C0 * (p.y - v0.y) + TopLeftEdge(v0, v1);
		}

		[[nodiscard]] __forceinline SSEInt EdgeFunc1(const SSEVec2i& p) const
		{
			return B1 * (p.x - v1.x) + C1 * (p.y - v1.y) + TopLeftEdge(v1, v2);
		}

		[[nodiscard]] __forceinline SSEInt EdgeFunc2(const SSEVec2i& p) const
		{
			return B2 * (p.x - v2.x) + C2 * (p.y - v2.y) + TopLeftEdge(v2, v0);
		}

		[[nodiscard]] __forceinline SSEBool Inside(const SSEVec2i& p) const
		{
			return (EdgeFunc0(p) | EdgeFunc1(p) | EdgeFunc2(p)) >= SSEInt(0);
		}

		[[nodiscard]] __forceinline bool TrivialReject(const SSEVec2i& p) const
		{
			return Any((EdgeFunc0(p) & EdgeFunc1(p) & EdgeFunc2(p)) < SSEInt(0));
		}

		[[nodiscard]] __forceinline SSEFloat GetDepth(float z0, float z1, float z2) const
		{
			const auto One = SSEFloat(1);
			const SSEFloat lambda2 = One - lambda0 - lambda1;
			return lambda0 * z0 + lambda1 * z1 + lambda2 * z2;
		}

		__forceinline void CalcBarycentricCoord(const SSEInt& x, const SSEInt& y)
		{
			lambda0 = SSEFloat((B1 * (x - v2.x) + C1 * (y - v2.y))) * invDet;
			lambda1 = SSEFloat((B2 * (x - v2.x) + C2 * (y - v2.y))) * invDet;
		}
	};
}
