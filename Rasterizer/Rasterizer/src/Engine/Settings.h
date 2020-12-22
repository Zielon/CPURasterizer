#pragma once

#include <cstdint>

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
	};
}
