#include <thread>

#include "Clipper.h"
#include "Scene.h"
#include "Camera.h"

namespace Engine
{
	Clipper::Clipper(const Scene& scene,
	                 const Camera& camera,
	                 const std::vector<Assets::Vertex>& vertices)
		: camera(camera), scene(scene), projectedVertexStorage(vertices)
	{
		const auto numCores = std::thread::hardware_concurrency();
		trianglesCount = scene.GetTriangleCount();
		coreInterval = (trianglesCount + numCores - 1) / numCores;

		sutherlandHodgman.reset(new SutherlandHodgman());
	}

	void Clipper::Clip(
		int bin, const Settings& settings, std::vector<Assets::Vertex>& clippedBuffer,
		std::vector<LarrabeeTriangle>& outTriangleBuffer) const
	{
		// Assign each thread to separate chunk of buffer
		const auto startIdx = bin * coreInterval;
		const auto endIdx = (bin + 1) * coreInterval;

		std::array<uint32_t, 12> clipped{};
		auto viewportMatrix = camera.GetRasterMatrix();

		for (uint32_t i = startIdx; i < endIdx; ++i)
		{
			if (3 * i + 2 > scene.GetIndexBuffer().size())
				break;

			auto v0 = projectedVertexStorage[scene.GetIndexBuffer()[3 * i + 0]];
			auto v1 = projectedVertexStorage[scene.GetIndexBuffer()[3 * i + 2]];
			auto v2 = projectedVertexStorage[scene.GetIndexBuffer()[3 * i + 1]];

			uint32_t clipCode0 = sutherlandHodgman->GetClipCode(v0.projectedPosition);
			uint32_t clipCode1 = sutherlandHodgman->GetClipCode(v1.projectedPosition);
			uint32_t clipCode2 = sutherlandHodgman->GetClipCode(v2.projectedPosition);

			if (clipCode0 | clipCode1 | clipCode2)
			{
				// Check the clipping codes correctness
				if (!(clipCode0 & clipCode1 & clipCode2))
				{
					auto polygon = sutherlandHodgman->ClipTriangle(
						v0.projectedPosition,
						v1.projectedPosition,
						v2.projectedPosition,
						(clipCode0 ^ clipCode1) | (clipCode1 ^ clipCode2) | (clipCode2 ^ clipCode0));

					for (int j = 0; j < polygon.Size(); j++)
					{
						auto weight = polygon[j].distances;

						if (weight.x == 1.0f)
						{
							v0.PerspectiveDivision();
							clipped[j] = clippedBuffer.size();
							clippedBuffer.push_back(v0);
						}
						else if (weight.y == 1.0f)
						{
							v1.PerspectiveDivision();
							clipped[j] = clippedBuffer.size();
							clippedBuffer.push_back(v1);
						}
						else if (weight.z == 1.0f)
						{
							v2.PerspectiveDivision();
							clipped[j] = clippedBuffer.size();
							clippedBuffer.push_back(v2);
						}
						else
						{
							auto vertex = v0 * weight.x + v1 * weight.y + v2 * weight.z;
							vertex.projectedPosition = polygon[j].pos;

							vertex.PerspectiveDivision();
							clipped[j] = clippedBuffer.size();
							clippedBuffer.push_back(vertex);
						}
					}

					for (int j = 2; j < polygon.Size(); j++)
					{
						// Preserve winding
						auto& id0 = clipped[0];
						auto& id1 = clipped[j - 1];
						auto& id2 = clipped[j];

						// Perspective division and viewport transformation
						glm::vec4 r0 = viewportMatrix * clippedBuffer[id0].projectedPosition;
						glm::vec4 r1 = viewportMatrix * clippedBuffer[id1].projectedPosition;
						glm::vec4 r2 = viewportMatrix * clippedBuffer[id2].projectedPosition;

						auto triId = outTriangleBuffer.size();

						LarrabeeTriangle triangle(r0, r1, r2, triId, bin, 0, { id0, id1, id2 });

						if (triangle.Setup(settings.cullBackFaces))
						{
							outTriangleBuffer.push_back(triangle);
						}
					}
				}

				continue;
			}

			v0.PerspectiveDivision();
			v1.PerspectiveDivision();
			v2.PerspectiveDivision();

			// Triangle is entirely inside viewing frustum 
			uint32_t id0 = clippedBuffer.size();
			clippedBuffer.push_back(v0);
			uint32_t id1 = clippedBuffer.size();
			clippedBuffer.push_back(v1);
			uint32_t id2 = clippedBuffer.size();
			clippedBuffer.push_back(v2);

			// Perspective division and viewport transformation
			glm::vec4 r0 = viewportMatrix * clippedBuffer[id0].projectedPosition;
			glm::vec4 r1 = viewportMatrix * clippedBuffer[id1].projectedPosition;
			glm::vec4 r2 = viewportMatrix * clippedBuffer[id2].projectedPosition;

			auto triId = outTriangleBuffer.size();

			LarrabeeTriangle triangle(r0, r1, r2, triId, bin, 0, { id0, id1, id2 });

			if (triangle.Setup(settings.cullBackFaces))
			{
				outTriangleBuffer.push_back(triangle);
			}
		}
	}
}
