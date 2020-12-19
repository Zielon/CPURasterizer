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

			normal = Normalize(normal);

			AVXVec3f lightDir = AVXVec3f(normalize(light));

			AVXFloat a = Dot(lightDir, normal);
			AVXBool mask = a < AVXFloat(0);
			a = AVX::Select(mask, AVXFloat(0), a);

			AVXFloat b = Dot(lightDir, normal * -1.f);
			mask = b < AVXFloat(0);
			b = AVX::Select(mask, AVXFloat(0), b);

			return a + b;
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
