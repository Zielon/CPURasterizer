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
		                       glm::vec3 eye, glm::vec3 light,
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
		               glm::vec3 eye,
		               glm::vec3 light,
		               Pixel& pixel) override
		{
			const Assets::Material& material = scene.GetMaterials()[pixel.materialId];

			AVXVec3f position;
			AVXVec3f normal;
			AVXVec2f texCoord;

			pixel.Interpolate(v0, v1, v2, pixel.lambda0, pixel.lambda1, position, normal, texCoord);

			// TODO Front facing normals
			normal = Normalize(normal);
			float lightIntensity = 3.f;

			AVXVec3f lightDir = AVXVec3f(normalize(light));
			AVXFloat diffuse = Dot(lightDir, normal);
			AVXBool mask = diffuse < AVXFloat(0);
			diffuse = AVX::Select(mask, AVXFloat(0), diffuse);
			AVXVec3f color = AVXVec3f((diffuse + 0.2f) * lightIntensity) * material.albedo * INVPI;

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
		               glm::vec3 eye,
		               glm::vec3 light,
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
