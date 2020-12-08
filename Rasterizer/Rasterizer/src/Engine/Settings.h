#pragma once

#include <cstdint>

constexpr int TILE = 5;
constexpr int TILE_SIZE = 1 << TILE;
constexpr int WIDTH = 1024;
constexpr int HEIGHT = 1024;

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
