#include "SutherlandHodgman.h"

#include <functional>
#include <stdexcept>

namespace Engine
{
	uint32_t INSIDE_BIT = 0;
	uint32_t LEFT_BIT = 1 << 0;
	uint32_t RIGHT_BIT = 1 << 1;
	uint32_t BOTTOM_BIT = 1 << 2;
	uint32_t TOP_BIT = 1 << 3;
	uint32_t FAR_BIT = 1 << 5;
	uint32_t NEAR_BIT = 1 << 4;

	Polygon SutherlandHodgman::ClipTriangle(
		const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2, uint32_t code)
	{
		Polygon polygon{};
		polygon.SetFromTriangle(v0, v1, v2);

		// ===================================================================== //

		if (code & LEFT_BIT)
			polygon = ClipPlane(
				LEFT_BIT, polygon,
				[](const glm::vec4& v) { return v.x >= -v.w; },
				[](glm::vec4& v) { v.x = -v.w; });

		if (code & RIGHT_BIT)
			polygon = ClipPlane(
				RIGHT_BIT, polygon,
				[](const glm::vec4& v) { return v.x <= v.w; },
				[](glm::vec4& v) { v.x = v.w; });

		// ===================================================================== //

		if (code & BOTTOM_BIT)
			polygon = ClipPlane(
				BOTTOM_BIT, polygon,
				[](const glm::vec4& v) { return v.y >= -v.w; },
				[](glm::vec4& v) { v.y = -v.w; });

		if (code & TOP_BIT)
			polygon = ClipPlane(
				TOP_BIT, polygon,
				[](const glm::vec4& v) { return v.y <= v.w; },
				[](glm::vec4& v) { v.y = v.w; });

		// ===================================================================== //

		if (code & FAR_BIT)
			polygon = ClipPlane(
				FAR_BIT, polygon,
				[](const glm::vec4& v) { return v.z <= v.w; },
				[](glm::vec4& v) { v.z = v.w; });

		if (code & NEAR_BIT)
			polygon = ClipPlane(
				NEAR_BIT, polygon,
				[](const glm::vec4& v) { return v.z >= 0.f; },
				[](glm::vec4& v) { v.z = 0.f; });

		// ===================================================================== //

		for (int i = 0; i < polygon.Size(); i++)
		{
			if (polygon[i].pos.w <= 0.0f)
			{
				polygon.Clear();
				break;
			}
		}

		return polygon;
	}

	__forceinline Polygon SutherlandHodgman::ClipPlane(
		uint32_t plane, const Polygon& inPolygon, const Predicate& isInside, const Clip& clip)
	{
		Polygon outPolygon;

		// For each edge/segment
		for (uint32_t i = 0, j = 1; i < inPolygon.Size(); ++i, ++j)
		{
			if (j == inPolygon.Size()) j = 0;

			// Point A on the segment
			const auto& aPos = inPolygon[i].pos;
			const auto& aDist = inPolygon[i].distances;

			// Point B on the segment
			const auto& bPos = inPolygon[j].pos;
			const auto& bDist = inPolygon[j].distances;

			float t = Point2PlaneDistance(plane, aPos, bPos);
			glm::vec4 newPos = aPos * (1 - t) + bPos * t; // Lerp
			const glm::vec3 newDist = aDist * (1 - t) + bDist * t; // Lerp
			clip(newPos);

			if (isInside(aPos))
			{
				if (isInside(bPos))
					outPolygon.Add(Polygon::Point{ bPos, bDist });
				else
					outPolygon.Add(Polygon::Point{ newPos, newDist });
			}
			else if (isInside(bPos))
			{
				outPolygon.Add(Polygon::Point{ newPos, newDist });
				outPolygon.Add(Polygon::Point{ bPos, bDist });
			}
		}

		return outPolygon;
	}

	__forceinline float SutherlandHodgman::Dot(uint32_t planeCode, const glm::vec4& v)
	{
		if (planeCode & LEFT_BIT) return v.x + v.w; /* v * (1 0 0 1)  */
		if (planeCode & RIGHT_BIT) return v.x - v.w; /* v * (-1 0 0 1) */
		if (planeCode & BOTTOM_BIT) return v.y + v.w; /* v * (0 -1 0 1) */
		if (planeCode & TOP_BIT) return v.y - v.w; /* v * (0 1 0 1)  */
		if (planeCode & FAR_BIT) return v.z - v.w; /* v * (0 0 -1 0) */
		if (planeCode & NEAR_BIT) return v.z; /* v * (0 0 1 1)  */

		return INFINITY;
	}

	uint32_t SutherlandHodgman::GetClipCode(const glm::vec4& v)
	{
		uint32_t code = INSIDE_BIT;

		if (v.x < -v.w) code |= LEFT_BIT;
		if (v.x > v.w) code |= RIGHT_BIT;
		if (v.y < -v.w) code |= BOTTOM_BIT;
		if (v.y > v.w) code |= TOP_BIT;
		if (v.z > v.w) code |= FAR_BIT;
		if (v.z < 0.0f)code |= NEAR_BIT;

		return code;
	}

	__forceinline float SutherlandHodgman::Point2PlaneDistance(
		uint32_t clipPlane, const glm::vec4& a, const glm::vec4& b)
	{
		return Dot(clipPlane, a) / (Dot(clipPlane, a) - Dot(clipPlane, b));
	}
}
