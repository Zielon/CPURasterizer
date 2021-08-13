#pragma once

#include <cstdint>
#include <glm/vec2.hpp>

constexpr int FIXED_POINT = 4;
constexpr int TILE = 5;
constexpr int TILE_SIZE = 1 << TILE;

namespace Engine
{
	struct Settings final
	{
		bool FXAA{};
		int lightModelId = 1;
		int textureFilterId{};
		int sceneId = 5;
		uint32_t trianglesCount{};
		uint32_t spheresCount{};
		float fps{};
		bool useShadows{};
		bool useGammaCorrection = true;
		bool cullBackFaces = true;
		glm::ivec2 resolution{};
	};
}
