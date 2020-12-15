#pragma once
#include "../../Math/Math.h"

namespace Engine
{
	class FragmentShader
	{
	public:
		virtual ~FragmentShader() {};
		virtual AVXVec3f Shade(Assets::Vertex& v0, Assets::Vertex& v1, Assets::Vertex& v2, Pixel& pixel) = 0;
	};

	class PhongBlinnShader : public FragmentShader
	{
	public:
		AVXVec3f Shade(Assets::Vertex& v0, Assets::Vertex& v1, Assets::Vertex& v2, Pixel& pixel) override
		{
			return AVXVec3f();
		}
	};

	class NormalsShader : public FragmentShader
	{
	public:
		AVXVec3f Shade(Assets::Vertex& v0, Assets::Vertex& v1, Assets::Vertex& v2, Pixel& pixel) override
		{
			AVXVec3f position;
			AVXVec3f normal;
			AVXVec2f texCoord;

			pixel.Interpolate(v0, v1, v2, pixel.lambda0, pixel.lambda1, position, normal, texCoord);
			AVXFloat w = Math::Rsqrt(Dot(normal, normal));
			AVXVec3f _normal = normal * w;
			return absVec(_normal);
		}
	};
}
