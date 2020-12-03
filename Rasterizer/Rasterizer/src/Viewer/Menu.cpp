#include "Menu.h"

#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"

namespace Viewer
{
	Menu::Menu() {}
	Menu::~Menu() {}

	void Menu::Render()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			const float distance = 5.0f;
			const ImVec2 pos = ImVec2(distance, distance);
			const ImVec2 posPivot = ImVec2(0.0f, 0.0f);
			const auto flags =
				ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoSavedSettings;

			ImGui::SetNextWindowPos(pos, ImGuiCond_Always, posPivot);
			ImGui::SetNextWindowSize(ImVec2(185, 450), ImGuiCond_Always);

			ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
			ImGui::Text("CPU Rasterizer");
			ImGui::Separator();
			ImGui::End();
		}			
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}
