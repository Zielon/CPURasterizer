#pragma once

#include <memory>

#define PARALLEL

#include "Clipper.h"
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
		/**
		 * \brief Clear all buffer before rendering a next frame.
		 */
		void Clear();

		/**
		 * \brief Run vertex shader on all vertices.
		 */
		void VertexShaderStage();

		/**
		 * \brief Clip all triangles to the viewing frustum.
		 * In the case of a triangle overlapping with one of the planes of the frustum
		 * the methods creates new triangles and add them to the buffer.
		 */
		void ClippingStage();

		/**
		 * \brief Tile or binning rasterization. Assigns all triangles to the proper overlapping tile.
		 */
		void TiledRasterizationStage();

		/**
		 * \brief Rasterize triangles using z-buffer visibility test.
		 */
		void RasterizationStage();

		/**
		 * \brief Run pixels shader on every generate fragment/pixel.
		 */
		void FragmentShaderStage();

		void UpdateFrameBuffer();
		void UpdateState(const Settings& settings);
		void CopyPixelsToBuffer();
		void CreateTiles();
		void CreateBuffers();

		const Scene& scene;
		const Camera& camera;

		int width;
		int height;
		int tileDimX;
		int tileDimY;

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
		std::unique_ptr<Clipper> clipper;
		std::unique_ptr<ColorBuffer> colorBuffer;
		std::unique_ptr<DepthBuffer> depthBuffer;
	};
}
