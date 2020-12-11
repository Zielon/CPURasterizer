#pragma once

#include <cstdint>

#include <glm/glm.hpp>

#include "Triangle.h"

#include "../Assets/Vertex.h"

namespace Engine
{
	class Clipper final
	{
	public:
		Clipper(const class Scene& scene, const class Camera& camera, const std::vector<Assets::Vertex>& vertices);
		
		void Clip(int bin, std::vector<Assets::Vertex>& clippedBuffer, std::vector<LarrabeeTriangle>& triangles) const;
	private:
		uint32_t trianglesCount;
		uint32_t coreInterval;
		const Camera& camera;
		const Scene& scene;
		const std::vector<Assets::Vertex>& projectedVertexStorage;

		static uint32_t GetClipCode(const glm::vec4& v);
	};
}
