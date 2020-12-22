#pragma once

#include "../Assets/Vertex.h"
#include "../Math/Math.h"
#include "../SIMD/AVX.h"
#include "../SIMD/SSE.h"

namespace Engine
{
	struct CoverageMask
	{
	private:
		int bits[8]{}; // For up to 32x MSAA coverage mask

	public:
		CoverageMask(const AVXBool& mask)
		{
			SetBit(mask);
		}

		void SetBit(const AVXBool& mask)
		{
			for (int i = 0; i < 8; ++i)
				if (mask[i] != 0)
					bits[i] = 1;
		}

		[[nodiscard]] int GetBit(int i) const
		{
			return bits[i];
		}
	};

	class Pixel
	{
	public:
		AVXFloat lambda0, lambda1;
		CoverageMask coverageMask;

		unsigned short x, y;
		uint32_t vId0, vId1, vId2, coreId;
		uint32_t materialId;
		uint32_t tileId;
		uint32_t intraTileIdx;

		Pixel(const AVXFloat& l0,
		      const AVXFloat& l1,
		      const int id0,
		      const int id1,
		      const int id2,
		      const int cId,
		      const int materialId,
		      const glm::ivec2& pixelCoord,
		      const CoverageMask& mask,
		      const int tId,
		      const uint32_t intraTId)
			: lambda0(l0)
			  , lambda1(l1)
			  , coverageMask(mask)
			  , x(pixelCoord.x)
			  , y(pixelCoord.y)
			  , vId0(id0)
			  , vId1(id1)
			  , vId2(id2)
			  , coreId(cId)
			  , materialId(materialId)
			  , tileId(tId)
			  , intraTileIdx(intraTId) { }


		/**
		 * \brief Uses Perspective Correct Vertex Attribute Interpolation
		 * \param v0 Vertex of a triangle
		 * \param v1 Vertex of a triangle
		 * \param v2 Vertex of a triangle
		 * \param b0 Barycentric coordinates v0
		 * \param b1 Barycentric coordinates v1
		 * \param position inout
		 * \param normal inout
		 * \param texCoord inout
		 */
		void Interpolate(Assets::Vertex& v0,
		                 Assets::Vertex& v1,
		                 Assets::Vertex& v2,
		                 AVXFloat& b0,
		                 AVXFloat& b1,
		                 AVXVec3f& position,
		                 AVXVec3f& normal,
		                 AVXVec2f& texCoord)
		{
			// Perspective Correct Vertex Attribute Interpolation
			const auto One = AVXFloat(1);
			AVXFloat b2 = One - b0 - b1;
			b0 = b0 * v0.invW;
			b1 = b1 * v1.invW;
			b2 = b2 * v2.invW;

			AVXFloat invB = One / (b0 + b1 + b2);
			b0 = b0 * invB;
			b1 = b1 * invB;
			b2 = One - b0 - b1;

			position = b0 * AVXVec3f(v0.position) + b1 * AVXVec3f(v1.position) + b2 * AVXVec3f(v2.position);
			normal = b0 * AVXVec3f(v0.normal) + b1 * AVXVec3f(v1.normal) + b2 * AVXVec3f(v2.normal);
			texCoord = b0 * AVXVec2f(v0.texCoords) + b1 * AVXVec2f(v1.texCoords) + b2 * AVXVec2f(v2.texCoords);
		}
	};
}
