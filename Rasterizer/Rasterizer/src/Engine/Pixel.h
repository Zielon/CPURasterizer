#pragma once

#include "../Assets/Vertex.h"
#include "../SIMD/SSE.h"

namespace Engine
{
	struct CoverageMask
	{
		int bits[4]; // For up to 32x MSAA coverage mask
		CoverageMask()
		{
			bits[0] = 0;
			bits[1] = 0;
			bits[2] = 0;
			bits[3] = 0;
		}

		CoverageMask(const SSEBool& mask, uint32_t sampleId)
			: CoverageMask()
		{
			SetBit(mask, sampleId);
		}

		void SetBit(int i)
		{
			int id = i >> 5;
			int shift = i & 31;
			bits[id] |= (1 << shift);
		}

		void SetBit(const SSEBool& mask, uint32_t sampleId)
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
		}

		int GetBit(int i) const
		{
			int id = i >> 5;
			int shift = i & 31;
			return bits[id] & (1 << shift);
		}

		int Merge() const
		{
			return bits[0] | bits[1] | bits[2] | bits[3];
		}
	};

	class Pixel
	{
	public:
		SSEFloat lambda0, lambda1;
		CoverageMask coverageMask;

		unsigned short x, y;
		uint32_t vId0, vId1, vId2, coreId;
		uint32_t textureId;
		uint32_t tileId;
		uint32_t intraTileIdx;

		Pixel(const SSEFloat& l0,
		      const SSEFloat& l1,
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
		                 SSEFloat& b0,
		                 SSEFloat& b1,
		                 SSEVec3f& position,
		                 SSEVec3f& normal,
		                 SSEVec2f& texCoord)
		{
			const auto One = SSEFloat(1);
			SSEFloat b2 = One - b0 - b1;
			b0 = b0 * v0.invW;
			b1 = b1 * v1.invW;
			b2 = b2 * v2.invW;
			SSEFloat invB = One / (b0 + b1 + b2);
			b0 = b0 * invB;
			b1 = b1 * invB;
			b2 = One - b0 - b1;

			position = b0 * SSEVec3f(v0.position.x, v0.position.y, v0.position.z) +
				b1 * SSEVec3f(v1.position.x, v1.position.y, v1.position.z) +
				b2 * SSEVec3f(v2.position.x, v2.position.y, v2.position.z);
			normal = b0 * SSEVec3f(v0.normal.x, v0.normal.y, v0.normal.z) +
				b1 * SSEVec3f(v1.normal.x, v1.normal.y, v1.normal.z) +
				b2 * SSEVec3f(v2.normal.x, v2.normal.y, v2.normal.z);
			texCoord = b0 * SSEVec2f(v0.texCoords.x, v0.texCoords.y) +
				b1 * SSEVec2f(v1.texCoords.x, v1.texCoords.y) +
				b2 * SSEVec2f(v2.texCoords.x, v2.texCoords.y);
		}
	};
}
