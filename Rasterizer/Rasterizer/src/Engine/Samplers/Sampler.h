#pragma once

#define GLM_FORCE_INLINE 
#include <glm/glm.hpp>

#include "../../Assets/Texture.h"
#include "../../Assets/Material.h"

namespace Engine
{
	class Sampler
	{
	public:
		Sampler(const class Scene& scene): scene(scene) {};

		glm::vec3 Sample(const glm::vec2& texCoord, const Assets::Material& material, Assets::TextureType textureType) const;
	protected:
		const Scene& scene;

		glm::vec3 Sample(const glm::vec2& texCoord, const Assets::Texture& texture) const;
	};
}
