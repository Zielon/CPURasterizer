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

		[[nodiscard]] glm::vec3 Sample(const glm::vec2& texCoord, const Assets::Material& material,
		                               Assets::TextureType textureType) const;
	protected:
		const Scene& scene;

		[[nodiscard]] glm::vec3 HDR(const glm::vec2& texCoord) const;
		[[nodiscard]] glm::vec3 Sample(const glm::vec2& texCoord, const Assets::Texture& texture) const;
		[[nodiscard]] glm::vec3 Nearest(float wrappedU, float wrappedV, const Assets::Texture& texture) const;
		[[nodiscard]] glm::vec3 Linear(float wrappedU, float wrappedV, const Assets::Texture& texture) const;
		[[nodiscard]] glm::vec3 Trilinear(float wrappedU, float wrappedV, const Assets::Texture& texture) const;
	};
}
