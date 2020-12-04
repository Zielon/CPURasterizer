#pragma once

#include "Settings.h"

namespace Engine
{
	class Renderer final
	{
	public:
		void Render(const Settings& settings);

	private:
		Settings settings{};
	};
}
