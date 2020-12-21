#pragma once

#include "../../Assets/Material.h"
#include "../../Math/Math.h"
#include "../Scene.h"
#include "../Camera.h"
#include "../Samplers/Sampler.h"

namespace Engine
{
	class FragmentShader
	{
	public:
		FragmentShader(const Scene& scene): scene(scene)
		{
			sampler.reset(new Sampler(scene));
		}

		virtual ~FragmentShader() = default;;

		/**
		 * \brief Perform shading in the world space
		 * \param v0 
		 * \param v1 
		 * \param v2 
		 * \param pixel 
		 * \return Color of 8 pixels
		 */
		virtual AVXVec3f Shade(Assets::Vertex& v0,
		                       Assets::Vertex& v1,
		                       Assets::Vertex& v2,
		                       Pixel& pixel) = 0;
	protected:
		const Scene& scene;
		std::unique_ptr<Sampler> sampler;
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

			float shininess = 32.0f;

			AVXVec3f color;
			AVXVec3f eye = AVXVec3f(scene.GetCamera().GetPosition());
			AVXVec3f viewDir = Normalize(eye - position);

			AVXVec3f albedo;

			if (material.albedoTexID != -1)
				for (auto i = 0; i < 8; i++)
				{
					glm::vec3 rgb = sampler->Sample({ texCoord.x[i], texCoord.y[i] }, material, Assets::Albedo);
					albedo.x[i] = rgb.r;
					albedo.y[i] = rgb.g;
					albedo.z[i] = rgb.b;
				}
			else
				albedo = AVXVec3f(material.albedo);

			for (const auto& light : lights)
			{
				glm::vec3 lightPos;
				if (light.type == 0) // Area light
					lightPos = light.position + 0.5f * light.v + 0.5f * light.u;
				else
					lightPos = light.position;

				AVXVec3f lightDir = Normalize(AVXVec3f(lightPos) - position);

				// cosTheta
				AVXFloat cos = Dot(normal, lightDir);
				AVXBool mask = cos < AVXFloat(0);
				AVXFloat cosTheta = AVX::Select(mask, AVXFloat(0), cos);

				AVXVec3f reflectDir = Reflect(lightDir * -1.f, normal);

				cos = Dot(reflectDir, viewDir);
				mask = cos < AVXFloat(0);
				AVXFloat cosBeta = AVX::Select(mask, AVXFloat(0), cos);

				AVXFloat spec = AVXFloat(
					pow(cosBeta[0], shininess),
					pow(cosBeta[1], shininess),
					pow(cosBeta[2], shininess),
					pow(cosBeta[3], shininess),
					pow(cosBeta[4], shininess),
					pow(cosBeta[5], shininess),
					pow(cosBeta[6], shininess),
					pow(cosBeta[7], shininess));

				auto ambient = albedo * 0.1f;
				auto diffuse = AVXVec3f(cosTheta) * albedo * 0.8f;
				auto specular = AVXVec3f(spec) * albedo * 0.9f;

				color = color + ambient + diffuse + specular;
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
