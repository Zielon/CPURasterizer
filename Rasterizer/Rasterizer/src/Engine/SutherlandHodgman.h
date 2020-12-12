#pragma once

#include <functional>
#include <vector>
#include <glm/glm.hpp>

namespace Engine
{
	using Predicate = std::function<bool(const glm::vec4&)>;
	using Intersection = std::function<float(const glm::vec4&, const glm::vec4&)>;
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
			glm::vec3 clipWeights;
		};

		std::vector<Point> points;

		[[nodiscard]] uint32_t Size() const { return points.size(); }

		void SetFromTriangle(const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2)
		{
			points.emplace_back(Point{ v0, { 1, 0, 0 } });
			points.emplace_back(Point{ v1, { 0, 1, 0 } });
			points.emplace_back(Point{ v2, { 0, 0, 1 } });
		}
	};

	/**
	 * \brief The Sutherland–Hodgman algorithm is an algorithm used for clipping polygons.
	 * It works by extending each line of the convex clip polygon in turn and
	 * selecting only vertices from the subject polygon that are on the visible side.
	 */
	class SutherlandHodgman
	{
	public:
		Polygon ClipTriangle(const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2, uint32_t code);
		uint32_t GetClipCode(const glm::vec4& v);
	private:
		Polygon ClipPlane(const Polygon&, const Predicate&, const Intersection&, const Clip&);
		float Dot(int clipPlane, const glm::vec4& v);
	};
}
