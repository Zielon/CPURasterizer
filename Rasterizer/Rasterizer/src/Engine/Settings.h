#pragma once

#include <cstdint>

namespace Engine
{
	struct Settings final
	{
		int MSSA{};
		int lightModelId{};
		int sceneId{};
		uint32_t trianglesCount{};
		uint32_t spheresCount{};
		float fps{};
		bool useShadows;
	};
}
