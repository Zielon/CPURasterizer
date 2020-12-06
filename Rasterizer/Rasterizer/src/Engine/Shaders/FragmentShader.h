#pragma once

namespace Engine
{
	class FragmentShader
	{
	public:
		virtual ~FragmentShader() {};
		virtual void Shade() = 0;
	};

	class PhongBlinnShader : public FragmentShader
	{
	public:
		PhongBlinnShader() {}
		~PhongBlinnShader() {}
		void Shade() override {}
	};
}
