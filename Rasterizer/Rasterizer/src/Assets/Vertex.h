#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <glm/glm.hpp>

namespace Assets
{
	struct Vertex final
	{
		// Projected position in homogenous space for clipping
		glm::vec4 projectedPosition{};

		// Index with relative position to all vertices buffer
		uint32_t id;

		float invW{};
		glm::vec3 position{};
		glm::vec3 normal{};
		glm::vec2 texCoords{};

		bool operator==(const Vertex& other) const
		{
			return position == other.position &&
				normal == other.normal &&
				texCoords == other.texCoords;
		}
	};
}

/**
 * The vertices vector contains a lot of duplicated vertex data,
 * because many vertices are included in multiple triangles.
 * We should keep only the unique vertices and use
 * the index buffer to reuse them whenever they come up.
 * https://en.cppreference.com/w/cpp/utility/hash
 */
namespace std
{
	template <>
	struct hash<Assets::Vertex>
	{
		size_t operator()(Assets::Vertex const& vertex) const
		{
			return
				((hash<glm::vec3>()(vertex.position) ^
					(hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoords) << 1);
		}
	};
}
