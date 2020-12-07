#pragma once

#include <array>
#include <memory>
#include <thread>

#include "Settings.h"
#include "Tile.h"
#include "Triangle.h"

#include "../Assets/Color4b.h"
#include "../Assets/Vertex.h"

#include "Shaders/FragmentShader.h"

namespace Engine
{
	class Renderer final
	{
	public:
		Renderer(const class Scene& scene, const class Camera& camera);

		void Render(const Settings& settings);

		[[nodiscard]] const uint8_t* GetFrameBuffer() const;

	private:
		void Clear();
		void VertexShaderStage();
		void ClippingStage();
		void TiledRasterizationStage();
		void FragmentShaderStage();
		void UpdateFrameBuffer();
		void UpdateState(const Settings& settings);

		void CreateTiles();
		void CreateBuffers();

		const Scene& scene;
		const Camera& camera;

		Settings settings{};
		std::vector<Assets::Vertex> projectedVertexStorage;

		// Lock-free distributed structures. Each tread operates on its own bin.
		std::vector<std::deque<Assets::Vertex>> clippedProjectedVertexBuffer;
		std::vector<std::deque<Triangle>> rasterTrianglesBuffer;
		std::array<Assets::Color4b, WIDTH * HEIGHT> frameBuffer;

		uint32_t numCores{};
		std::vector<uint32_t> coreIds;
		std::vector<Tile> tiles;
		std::unique_ptr<FragmentShader> fragmentShader;
	};
}
