#pragma once

#include <memory>
#include <thread>

#include "Settings.h"
#include "Tile.h"
#include "LarrabeeRasterizer.h"
#include "Triangle.h"
#include "ColorBuffer.h"
#include "DepthBuffer.h"

#include "../Assets/Vertex.h"

#include "Shaders/FragmentShader.h"
#include "Shaders/VertexShader.h"

namespace Engine
{
	class Renderer final
	{
	public:
		Renderer(const class Scene& scene, const class Camera& camera);

		void Render(const Settings& settings);

		[[nodiscard]] const uint8_t* GetColorBuffer() const;

	private:
		void Clear();
		void VertexShaderStage();
		void ClippingStage();
		void TiledRasterizationStage();
		void RasterizationStage();
		void FragmentShaderStage();
		void UpdateFrameBuffer();
		void UpdateState(const Settings& settings);

		void CreateTiles();
		void CreateBuffers();

		const Scene& scene;
		const Camera& camera;

		Settings settings{};

		// Lock-free distributed structures. Each thread operates on its own bin.
		Buffer2D<Assets::Vertex> clippedProjectedVertexBuffer;
		Buffer2D<LarrabeeTriangle> rasterTrianglesBuffer;

		uint32_t numCores{};
		std::vector<Pixel> pixels;
		std::vector<std::vector<AVXInt>> tiledPixels;
		std::vector<Tile> tiles;
		std::vector<uint32_t> coreIds;
		std::vector<Assets::Vertex> projectedVertexStorage;

		std::unique_ptr<FragmentShader> fragmentShader;
		std::unique_ptr<VertexShader> vertexShader;
		std::unique_ptr<LarrabeeRasterizer> rasterizer;
		std::unique_ptr<ColorBuffer> colorBuffer;
		std::unique_ptr<DepthBuffer> depthBuffer;
	};
}
