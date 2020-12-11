#pragma once

#include "../Assets/Vertex.h"
#include "../SIMD/AVX.h"
#include "../SIMD/SSE.h"

namespace Engine
{
	struct CoverageMask
	{
		int bits[8]; // For up to 32x MSAA coverage mask
		CoverageMask()
		{
			bits[0] = 0;
			bits[1] = 0;
			bits[2] = 0;
			bits[3] = 0;
			bits[4] = 0;
			bits[5] = 0;
			bits[6] = 0;
			bits[7] = 0;
		}

		CoverageMask(const AVXBool& mask, uint32_t sampleId)
			: CoverageMask()
		{
			SetBit(mask, sampleId);
		}

		void SetBit(int i)
		{
			int shift = i & 31;
			bits[i] = 1;
		}

		void SetBit(const AVXBool& mask, uint32_t sampleId)
		{
			uint32_t sampleOffset = sampleId << 2;
			if (mask[0] != 0)
			{
				SetBit(sampleOffset + 0);
			}
			if (mask[1] != 0)
			{
				SetBit(sampleOffset + 1);
			}
			if (mask[2] != 0)
			{
				SetBit(sampleOffset + 2);
			}
			if (mask[3] != 0)
			{
				SetBit(sampleOffset + 3);
			}
			if (mask[4] != 0)
			{
				SetBit(sampleOffset + 4);
			}
			if (mask[5] != 0)
			{
				SetBit(sampleOffset + 5);
			}
			if (mask[6] != 0)
			{
				SetBit(sampleOffset + 6);
			}
			if (mask[7] != 0)
			{
				SetBit(sampleOffset + 7);
			}
		}

		int GetBit(int i) const
		{
			int shift = i & 31;
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
		uint32_t textureId;
		uint32_t tileId;
		uint32_t intraTileIdx;

		Pixel(const AVXFloat& l0,
		      const AVXFloat& l1,
		      const int id0,
		      const int id1,
		      const int id2,
		      const int cId,
		      const int texId,
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
			  , textureId(texId)
			  , tileId(tId)
			  , intraTileIdx(intraTId) { }

		void Interpolate(const Assets::Vertex& v0,
		                 const Assets::Vertex& v1,
		                 const Assets::Vertex& v2,
		                 AVXFloat& b0,
		                 AVXFloat& b1,
		                 AVXVec3f& position,
		                 AVXVec3f& normal,
		                 AVXVec2f& texCoord)
		{
			const auto One = AVXFloat(1);
			AVXFloat b2 = One - b0 - b1;
			b0 = b0 * v0.invW;
			b1 = b1 * v1.invW;
			b2 = b2 * v2.invW;
			AVXFloat invB = One / (b0 + b1 + b2);
			b0 = b0 * invB;
			b1 = b1 * invB;
			b2 = One - b0 - b1;

			position = b0 * AVXVec3f(v0.position.x, v0.position.y, v0.position.z) +
				b1 * AVXVec3f(v1.position.x, v1.position.y, v1.position.z) +
				b2 * AVXVec3f(v2.position.x, v2.position.y, v2.position.z);
			normal = b0 * AVXVec3f(v0.normal.x, v0.normal.y, v0.normal.z) +
				b1 * AVXVec3f(v1.normal.x, v1.normal.y, v1.normal.z) +
				b2 * AVXVec3f(v2.normal.x, v2.normal.y, v2.normal.z);
			texCoord = b0 * AVXVec2f(v0.texCoords.x, v0.texCoords.y) +
				b1 * AVXVec2f(v1.texCoords.x, v1.texCoords.y) +
				b2 * AVXVec2f(v2.texCoords.x, v2.texCoords.y);
		}
	};
}
