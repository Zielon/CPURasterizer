#pragma once

#include "../Camera.h"

namespace Engine
{
	class VertexShader
	{
	public:
		VertexShader(const Camera& camera) : camera(camera) {}
		virtual ~VertexShader() = default;;
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
			glm::mat4 model(1);
			
			outVertex.projectedPosition =
				camera.GetProjectionMatrix() * camera.GetViewMatrix() * model * glm::vec4(inVertex.position, 1.f);
			outVertex.position = inVertex.position;
			outVertex.normal = inVertex.normal;
			outVertex.texCoords = inVertex.texCoords;
			outVertex.id = inVertex.id;
		}
	};
}
