#pragma once

#include <imgui.h>

#include "../Engine/Settings.h"

namespace Viewer
{
	class Menu final
	{
	public:
		Menu(const class Window& window);
		~Menu();

		void Render();

		[[nodiscard]] Engine::Settings& GetSettings() { return settings; }

		static bool WantCaptureMouse()
		{
			return ImGui::GetIO().WantCaptureMouse;
		}

		static bool WantCaptureKeyboard()
		{
			return ImGui::GetIO().WantCaptureKeyboard;
		}

	private:
		Engine::Settings settings{};
	};
}
