#pragma once

#include <cstdint>

namespace Engine
{
	class Triangle
	{
	public:
		Triangle() = default;

		Triangle(const glm::ivec2& v0, const glm::ivec2& v1, const glm::ivec2& v2, uint32_t binId,
		         std::array<uint32_t, 3> ids): v0(v0), v1(v1), v2(v2), binId(binId), ids(ids) {}

		bool IsValid()
		{
			auto B1 = v1.y - v2.y;
			auto C1 = v2.x - v1.x;
			auto B2 = v2.y - v0.y;
			auto C2 = v0.x - v2.x;

			int det = C2 * B1 - C1 * B2;

			if (det <= 0)
				return false;

			return true;
		}

		float EdgeFunction(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
		{
			return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]);
		}

		glm::ivec2 v0{};
		glm::ivec2 v1{};
		glm::ivec2 v2{};
		uint32_t binId{};
		std::array<uint32_t, 3> ids;
	};
}
