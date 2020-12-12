#include <thread>

#include "Clipper.h"
#include "Scene.h"
#include "Camera.h"

namespace Engine
{
	Clipper::Clipper(const Scene& scene, const Camera& camera, const std::vector<Assets::Vertex>& vertices)
		: camera(camera), scene(scene), projectedVertexStorage(vertices)
	{
		const auto numCores = std::thread::hardware_concurrency();
		trianglesCount = scene.GetTriangleCount();
		coreInterval = (trianglesCount + numCores - 1) / numCores;

		sutherlandHodgman.reset(new SutherlandHodgman());
	}

	void Clipper::Clip(
		int bin, std::vector<Assets::Vertex>& clippedBuffer, std::vector<LarrabeeTriangle>& triangles) const
	{
		// Assign each thread to separate chunk of buffer
		const auto startIdx = bin * coreInterval;
		const auto endIdx = (bin + 1) * coreInterval;

		for (uint32_t i = startIdx; i < endIdx; ++i)
		{
			if (3 * i + 2 > scene.GetIndexBuffer().size())
				break;

			const auto& v0 = projectedVertexStorage[scene.GetIndexBuffer()[3 * i + 0]];
			const auto& v1 = projectedVertexStorage[scene.GetIndexBuffer()[3 * i + 2]];
			const auto& v2 = projectedVertexStorage[scene.GetIndexBuffer()[3 * i + 1]];

			uint32_t clipCode0 = sutherlandHodgman->GetClipCode(v0.projectedPosition);
			uint32_t clipCode1 = sutherlandHodgman->GetClipCode(v1.projectedPosition);
			uint32_t clipCode2 = sutherlandHodgman->GetClipCode(v2.projectedPosition);

			if (clipCode0 | clipCode1 | clipCode2)
			{
				if (!(clipCode0 & clipCode1 & clipCode2))
				{
					int clipVertIds[12];

					auto polygon = sutherlandHodgman->ClipTriangle(
						v0.projectedPosition,
						v1.projectedPosition,
						v2.projectedPosition,
						(clipCode0 ^ clipCode1) | (clipCode1 ^ clipCode2) | (clipCode2 ^ clipCode0));

					for (int j = 0; j < polygon.points.size(); j++)
					{
						auto weight = polygon.points[j].clipWeights;
					}
				}

				continue;
			}

			uint32_t idx0 = clippedBuffer.size();
			clippedBuffer.push_back(v0);
			uint32_t idx1 = clippedBuffer.size();
			clippedBuffer.push_back(v1);
			uint32_t idx2 = clippedBuffer.size();
			clippedBuffer.push_back(v2);

			// Perspective division
			float inv = 1.f / v0.projectedPosition.w;
			glm::vec4 hv0 = v0.projectedPosition * inv;

			inv = 1.f / v1.projectedPosition.w;
			glm::vec4 hv1 = v1.projectedPosition * inv;

			inv = 1.f / v2.projectedPosition.w;
			glm::vec4 hv2 = v2.projectedPosition * inv;

			auto raster = camera.GetRasterMatrix();

			glm::vec4 r0 = raster * hv0;
			glm::vec4 r1 = raster * hv1;
			glm::vec4 r2 = raster * hv2;

			auto triId = triangles.size();

			LarrabeeTriangle triangle(r0, r1, r2, triId, bin, 0, { idx0, idx1, idx2 });

			if (triangle.Setup())
			{
				triangles.push_back(triangle);
			}
		}
	}
}
