#pragma once

namespace Engine
{
	class VertexShader
	{
	public:
		virtual ~VertexShader() {};
		virtual void Execute() = 0;
	};

	class BasicVertexShader : public VertexShader
	{
	public:
		BasicVertexShader() {}
		~BasicVertexShader() {}
		void Execute() override {}
	};
}
