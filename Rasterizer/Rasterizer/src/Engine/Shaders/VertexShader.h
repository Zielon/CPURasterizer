#pragma once

#include "../Camera.h"

namespace Engine
{
	class VertexShader
	{
	public:
		VertexShader(const Camera& camera) : camera(camera) {}
		virtual ~VertexShader() = default;

		/**
		 * \brief Transformation per vertex
		 * \param inVertex Vertex before
		 * \param outVertex Vertex after
		 */
		virtual void Process(const Assets::Vertex& inVertex, Assets::Vertex& outVertex) = 0;
	protected:
		const Camera& camera;
	};

	class DefaultVertexShader : public VertexShader
	{
	public:
		DefaultVertexShader(const Camera& camera): VertexShader(camera) {}

		void Process(const Assets::Vertex& inVertex, Assets::Vertex& outVertex) override
		{
			outVertex.projectedPosition =
				camera.GetProjectionMatrix() * camera.GetViewMatrix() * glm::vec4(inVertex.position, 1.f);
			outVertex.position = inVertex.position;
			outVertex.normal = inVertex.normal;
			outVertex.texCoords = inVertex.texCoords;
			outVertex.id = inVertex.id;
			outVertex.materialId = inVertex.materialId;
		}
	};
}
