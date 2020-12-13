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

		auto intersection = [&](const glm::vec4& v0, const glm::vec4& v1) {
			return Dot(code, v0) / (Dot(code, v0) - Dot(code, v1));
		};

		if (code & LEFT_BIT)
			polygon = ClipPlane(
				polygon,
				[](const glm::vec4& v) { return v.x >= -v.w; },
				intersection,
				[](glm::vec4& v) { v.x = -v.w; });

		if (code & RIGHT_BIT)
			polygon = ClipPlane(
				polygon,
				[](const glm::vec4& v) { return v.x <= v.w; },
				intersection,
				[](glm::vec4& v) { v.x = v.w; });

		// ===================================================================== //

		if (code & BOTTOM_BIT)
			polygon = ClipPlane(
				polygon,
				[](const glm::vec4& v) { return v.y >= -v.w; },
				intersection,
				[](glm::vec4& v) { v.y = -v.w; });

		if (code & TOP_BIT)
			polygon = ClipPlane(
				polygon,
				[](const glm::vec4& v) { return v.y <= v.w; },
				intersection,
				[](glm::vec4& v) { v.y = v.w; });

		// ===================================================================== //

		if (code & FAR_BIT)
			polygon = ClipPlane(
				polygon,
				[](const glm::vec4& v) { return v.z <= v.w; },
				intersection,
				[](glm::vec4& v) { v.z = v.w; });

		if (code & NEAR_BIT)
			polygon = ClipPlane(
				polygon,
				[](const glm::vec4& v) { return v.z >= 0.f; },
				intersection,
				[](glm::vec4& v) { v.z = 0.f; });

		return polygon;
	}

	Polygon SutherlandHodgman::ClipPlane(
		const Polygon& inPolygon, const Predicate& isInside, const Intersection& T, const Clip& clip)
	{
		Polygon outPolygon;

		for (uint32_t i = 0, j = 1; i < inPolygon.Size(); ++i, ++j)
		{
			if (j == inPolygon.Size()) j = 0;

			const auto& currentPos = inPolygon.points[i].pos;
			const auto& currentDistance = inPolygon.points[i].distances;

			const auto& previousPos = inPolygon.points[j].pos;
			const auto& previousDistance = inPolygon.points[j].distances;

			float t = T(currentPos, previousPos);
			glm::vec4 newPosition = currentPos * (1 - t) + previousPos * t; // Lerp
			clip(newPosition);
			glm::vec3 newWeight = currentDistance * (1 - t) + previousDistance * t;

			if (isInside(currentPos))
			{
				if (isInside(previousPos))
					outPolygon.points.push_back(Polygon::Point{ previousPos, inPolygon.points[j].distances });
				else
					outPolygon.points.push_back(Polygon::Point{ newPosition, newWeight });
			}
			else if (isInside(previousPos))
			{
				outPolygon.points.push_back(Polygon::Point{ newPosition, newWeight });
				outPolygon.points.push_back(Polygon::Point{ previousPos, inPolygon.points[j].distances });
			}
		}

		return outPolygon;
	}

	float SutherlandHodgman::Dot(int planeCode, const glm::vec4& v)
	{
		if (planeCode & LEFT_BIT) return v.x + v.w; /* v * (1 0 0 1)  */
		if (planeCode & RIGHT_BIT) return -v.x + v.w; /* v * (-1 0 0 1) */
		if (planeCode & BOTTOM_BIT) return -v.y + v.w; /* v * (0 -1 0 1) */
		if (planeCode & TOP_BIT) return v.y + v.w; /* v * (0 1 0 1)  */
		if (planeCode & FAR_BIT) return -v.z; /* v * (0 0 -1 0) */
		if (planeCode & NEAR_BIT) return v.z + v.w; /* v * (0 0 1 1)  */

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
}
