#pragma once

#include <imgui.h>

namespace Engine
{
	struct Settings;
}

namespace Viewer
{
	class Menu final
	{
	public:
		Menu(const class Window& window);
		~Menu();

		void Render(Engine::Settings& settings) const;

		static bool WantCaptureMouse()
		{
			return ImGui::GetIO().WantCaptureMouse;
		}

		static bool WantCaptureKeyboard()
		{
			return ImGui::GetIO().WantCaptureKeyboard;
		}
	};
}
