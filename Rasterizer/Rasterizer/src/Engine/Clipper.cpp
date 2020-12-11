#include <thread>

#include "Clipper.h"
#include "Scene.h"
#include "Camera.h"

namespace Plane
{
	uint32_t INSIDE_BIT = 0;
	uint32_t LEFT_BIT = 1 << 0;
	uint32_t RIGHT_BIT = 1 << 1;
	uint32_t BOTTOM_BIT = 1 << 2;
	uint32_t TOP_BIT = 1 << 3;
	uint32_t FAR_BIT = 1 << 5;
	uint32_t NEAR_BIT = 1 << 4;
}

namespace Engine
{
	Clipper::Clipper(const Scene& scene, const Camera& camera, const std::vector<Assets::Vertex>& vertices)
		: camera(camera), scene(scene), projectedVertexStorage(vertices)
	{
		auto numCores = std::thread::hardware_concurrency();
		trianglesCount = scene.GetTriangleCount();
		coreInterval = (trianglesCount + numCores - 1) / numCores;
	}

	uint32_t Clipper::GetClipCode(const glm::vec4& v)
	{
		uint32_t code = Plane::INSIDE_BIT;

		if (v.x < -v.w)
			code |= Plane::LEFT_BIT;
		if (v.x > v.w)
			code |= Plane::RIGHT_BIT;
		if (v.y < -v.w)
			code |= Plane::BOTTOM_BIT;
		if (v.y > v.w)
			code |= Plane::TOP_BIT;
		if (v.z > v.w)
			code |= Plane::FAR_BIT;
		if (v.z < 0.0f)
			code |= Plane::NEAR_BIT;

		return code;
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

			uint32_t clipCode0 = GetClipCode(v0.projectedPosition);
			uint32_t clipCode1 = GetClipCode(v1.projectedPosition);
			uint32_t clipCode2 = GetClipCode(v2.projectedPosition);

			if (clipCode0 | clipCode1 | clipCode2) continue;

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
