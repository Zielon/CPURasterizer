#pragma once

#include "../../Assets/Material.h"
#include "../../Math/Math.h"
#include "../Scene.h"

namespace Engine
{
	class FragmentShader
	{
	public:
		FragmentShader(const Scene& scene): scene(scene) {}
		virtual ~FragmentShader() = default;;
		virtual AVXVec3f Shade(Assets::Vertex& v0,
		                       Assets::Vertex& v1,
		                       Assets::Vertex& v2,
		                       Pixel& pixel) = 0;
	protected:
		const Scene& scene;
	};

	class PhongBlinnShader : public FragmentShader
	{
	public:
		PhongBlinnShader(const Scene& scene): FragmentShader(scene) {}

		AVXVec3f Shade(Assets::Vertex& v0,
		               Assets::Vertex& v1,
		               Assets::Vertex& v2,
		               Pixel& pixel) override
		{
			const auto& material = scene.GetMaterials()[pixel.materialId];
			const auto& lights = scene.GetLights();

			AVXVec3f position;
			AVXVec3f normal;
			AVXVec2f texCoord;

			pixel.Interpolate(v0, v1, v2, pixel.lambda0, pixel.lambda1, position, normal, texCoord);

			normal = Normalize(normal);
			AVXVec3f color;

			for (const auto& light : lights)
			{
				glm::vec3 lightPos;
				if (light.type == 0) // Area light
					lightPos = light.position + 0.5f * light.v + 0.5f * light.u;
				else
					lightPos = light.position;

				AVXVec3f lightDir = Normalize(AVXVec3f(lightPos) - position);

				// cosTheta
				AVXFloat cos = Dot(lightDir, normal);
				AVXBool mask = cos < AVXFloat(0);
				AVXFloat cosTheta = AVX::Select(mask, AVXFloat(0), cos);

				auto ambient = AVXVec3f(material.albedo * 0.1f);
				auto diffuse = AVXVec3f(cosTheta) * material.albedo * 0.8f;

				color = color + ambient + diffuse;
			}

			return color;
		}
	};

	class NormalShader : public FragmentShader
	{
	public:
		NormalShader(const Scene& scene) : FragmentShader(scene) {}

		AVXVec3f Shade(Assets::Vertex& v0,
		               Assets::Vertex& v1,
		               Assets::Vertex& v2,
		               Pixel& pixel) override
		{
			AVXVec3f position;
			AVXVec3f normal;
			AVXVec2f texCoord;

			pixel.Interpolate(v0, v1, v2, pixel.lambda0, pixel.lambda1, position, normal, texCoord);

			normal = Normalize(normal);

			return Abs(normal);
		}
	};
}
