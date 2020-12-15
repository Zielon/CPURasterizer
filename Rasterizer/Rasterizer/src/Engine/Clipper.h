#pragma once

#include <cstdint>

#include "Triangle.h"
#include "SutherlandHodgman.h"

#include "../Assets/Vertex.h"

namespace Engine
{
	/**
	 * \brief Clips points the the viewing frustum planes.
	 * Additionally performs perspective division.
	 */
	class Clipper final
	{
	public:
		Clipper(const class Scene& scene, const class Camera& camera, const std::vector<Assets::Vertex>& vertices);

		/**
		 * \brief Clip triangles and projects to NDC using perspective division.
		 * \param bin Current bin for lock-free multi-threading.
		 * \param clippedBuffer Current buffer assigned for bin core id.
		 * \param outTriangleBuffer Output buffer of triangles inside viewing frustum.
		 */
		void Clip(int bin, const Settings& settings, std::vector<Assets::Vertex>& clippedBuffer,
		          std::vector<LarrabeeTriangle>& outTriangleBuffer) const;
	private:
		uint32_t trianglesCount;
		uint32_t coreInterval;

		const Camera& camera;
		const Scene& scene;
		const std::vector<Assets::Vertex>& projectedVertexStorage;

		std::unique_ptr<SutherlandHodgman> sutherlandHodgman;
	};
}
