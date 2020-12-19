#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Assets
{
	struct Material
	{
		glm::vec3 albedo;
		std::string texturePath = "";
		int textureId{};
	};
}
