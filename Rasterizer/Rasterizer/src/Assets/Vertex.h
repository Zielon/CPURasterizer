#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <glm/glm.hpp>

#include "../Config.h"

namespace Assets
{
	struct Vertex final
	{
		// Projected position in homogenous space for clipping
		glm::vec4 projectedPosition{};

		// Index with relative position to all vertices buffer
		uint32_t id;
		int materialId = -1;

		glm::vec3 position{};
		glm::vec3 normal{};
		glm::vec2 texCoords{};

		float invW{};

		void PerspectiveDivision()
		{
			invW = 1.f / projectedPosition.w;
			projectedPosition *= invW;
		}

		CFG_FORCE_INLINE Vertex operator*(float t) const
		{
			auto copy = *this;

			copy.position *= t;
			copy.normal *= t;
			copy.texCoords *= t;

			return copy;
		}

		CFG_FORCE_INLINE Vertex operator+(const Vertex& v) const
		{
			auto copy = *this;

			copy.position += v.position;
			copy.normal += v.normal;
			copy.texCoords += v.texCoords;

			return copy;
		}

		CFG_FORCE_INLINE bool operator==(const Vertex& v) const
		{
			return position == v.position && normal == v.normal && texCoords == v.texCoords;
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
