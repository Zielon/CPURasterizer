#include "Sampler.h"

#include "../Scene.h"
#include "../Camera.h"

namespace Engine
{
	glm::vec3 Sampler::Sample(
		const glm::vec2& texCoord,
		const Assets::Material& material,
		Assets::TextureType textureType) const
	{
		const auto& textures = scene.GetTextures();

		switch (textureType)
		{
		case Assets::Albedo:
			return Sample(texCoord, *textures[material.albedoTexID]);
		default:
			return {};
		}
	}

	inline int Floor(float x)
	{
		int i = static_cast<int>(x);
		return i - (i > x);
	}

	glm::vec3 Sampler::Sample(const glm::vec2& texCoord, const Assets::Texture& texture) const
	{
		auto wrappedU = texCoord.x - Floor(texCoord.x);
		auto wrappedV = texCoord.y - Floor(texCoord.y);

		int u = wrappedU * texture.GetWidth();
		int v = wrappedV * texture.GetHeight();

		u = std::clamp(u, 0, texture.GetWidth() - 1);
		v = std::clamp(v, 0, texture.GetHeight() - 1);
		auto* pixelOffset = texture.GetPixels().data() + (u + texture.GetWidth() * v) * 4;
		return { pixelOffset[0], pixelOffset[1], pixelOffset[2] };
	}
}
