#pragma once

#include <functional>
#include <vector>
#include <glm/glm.hpp>

#include "../Config.h"

namespace Engine
{
	using Predicate = std::function<bool(const glm::vec4&)>;
	using Clip = std::function<void(glm::vec4&)>;

	struct Polygon
	{
		Polygon()
		{
			// Mostly triangles
			points.reserve(3);
		}

		struct Point
		{
			glm::vec4 pos;
			// Distances used for linear interpolation between segment and frustum plane
			glm::vec3 distances;
		};

		[[nodiscard]] uint32_t Size() const { return points.size(); }
		CFG_FORCE_INLINE void Clear() { points.clear(); }
		CFG_FORCE_INLINE void Add(const Point& point) { points.push_back(point); }
		CFG_FORCE_INLINE Point& operator [](const size_t i) { return points[i]; }
		CFG_FORCE_INLINE const Point& operator [](const size_t i) const { return points[i]; }

		void SetFromTriangle(const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2)
		{
			points.emplace_back(Point{ v0, { 1, 0, 0 } });
			points.emplace_back(Point{ v1, { 0, 1, 0 } });
			points.emplace_back(Point{ v2, { 0, 0, 1 } });
		}

	private:
		std::vector<Point> points;
	};

	/**
	 * \brief The Sutherland�Hodgman algorithm is an algorithm used for clipping polygons.
	 * It works by extending each line of the convex clip polygon in turn and
	 * selecting only vertices from the subject polygon that are on the visible side.
	 * https://chaosinmotion.com/2016/05/22/3d-clipping-in-homogeneous-coordinates/
	 */
	class SutherlandHodgman
	{
	public:
		Polygon ClipTriangle(const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2, uint32_t code);
		uint32_t GetClipCode(const glm::vec4& v);
	private:
		/**
		 * \brief Computers code for all planes which a given point fails
		 * \param v Current point for clipping (after vertex shader)
		 * \return Code for planes to clip
		 */
		Polygon ClipPlane(uint32_t plane, const Polygon&, const Predicate&, const Clip&);

		/**
		 * \brief Clipping dot products
		 * \param clipPlane Bit for the intersection plane
		 * \param v Point for which we check intersection
		 * \return Distance to the given plane
		 */
		float Dot(uint32_t clipPlane, const glm::vec4& v);

		float Point2PlaneDistance(uint32_t clipPlane, const glm::vec4& a, const glm::vec4& b);
	};
}
