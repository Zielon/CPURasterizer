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
			// Left handed system
			//model[0][0] = 0.01f;
			//model[1][1] = 0.01f;
			//model[2][2] = 0.01f;

			//model[3][0] = .3685;
			//model[3][1] = .165;
			//model[3][2] = .35125;

			outVertex.projectedPosition =
				camera.GetProjectionMatrix() * camera.GetViewMatrix() * model * glm::vec4(inVertex.position, 1.f);
			outVertex.position = inVertex.position;
			outVertex.normal = inVertex.normal;
			outVertex.texCoords = inVertex.texCoords;
			outVertex.id = inVertex.id;
		}
	};
}
