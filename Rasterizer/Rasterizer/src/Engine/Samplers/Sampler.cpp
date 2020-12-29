#include "Sampler.h"

#include "../../Config.h"

#include "../Scene.h"
#include "../Renderer.h"

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
		case Assets::Height:
			return Sample(texCoord, *textures[material.heightmapTexID]);
		case Assets::Normal:
			return Sample(texCoord, *textures[material.normalmapTexID]);
		case Assets::Metallic:
			return Sample(texCoord, *textures[material.metallicRoughnessTexID]);
		default:
			return {};
		}
	}

	CFG_FORCE_INLINE int Floor(float x)
	{
		const int i = static_cast<int>(x);
		return i - (i > x);
	}

	glm::vec3 Sampler::Sample(const glm::vec2& texCoord, const Assets::Texture& texture) const
	{
		const auto wrappedU = texCoord.x - Floor(texCoord.x);
		const auto wrappedV = texCoord.y - Floor(texCoord.y);

		switch (Renderer::Instance()->GetSettings().textureFilterId)
		{
		case 0:
			return Nearest(wrappedU, wrappedV, texture);
		case 1:
			return Linear(wrappedU, wrappedV, texture);
		case 2:
			return Trilinear(wrappedU, wrappedV, texture);
		default:
			return {};
		}
	}

	glm::vec3 Sampler::Nearest(float wrappedU, float wrappedV, const Assets::Texture& texture) const
	{
		int u = wrappedU * texture.GetWidth();
		int v = wrappedV * texture.GetHeight();

		u = std::clamp(u, 0, texture.GetWidth() - 1);
		v = std::clamp(v, 0, texture.GetHeight() - 1);

		auto* pixelOffset = texture.GetPixels().data() + (u + texture.GetWidth() * v) * 4;
		return { pixelOffset[0], pixelOffset[1], pixelOffset[2] };
	}

	glm::vec3 Sampler::Linear(float wrappedU, float wrappedV, const Assets::Texture& texture) const
	{
		return {};
	}

	glm::vec3 Sampler::Trilinear(float wrappedU, float wrappedV, const Assets::Texture& texture) const
	{
		return {};
	}
}
