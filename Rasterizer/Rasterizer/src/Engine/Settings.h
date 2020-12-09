#pragma once

#include <cstdint>

constexpr int FIXED_POINT = 4;
constexpr int TILE = 5;
constexpr int TILE_SIZE = 1 << TILE;
constexpr int WIDTH = 800;
constexpr int HEIGHT = 800;
constexpr int TILE_DIM_X = (WIDTH + TILE_SIZE - 1) >> TILE;
constexpr int TILE_DIM_Y = (HEIGHT + TILE_SIZE - 1) >> TILE;

namespace Engine
{
	struct Settings final
	{
		int MSSA{};
		int lightModelId{};
		int textureFilterId{};
		int sceneId{};
		uint32_t trianglesCount{};
		uint32_t spheresCount{};
		float fpms{};
		bool useShadows;
	};
}
