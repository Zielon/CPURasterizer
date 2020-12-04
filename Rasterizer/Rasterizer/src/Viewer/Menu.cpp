#include "Menu.h"

#include <thread>

#include "Window.h"

#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"

#include "../Engine/Settings.h"

namespace Viewer
{
	Menu::Menu(const Window& window)
	{
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::StyleColorsDark();
		ImGui::GetStyle().Alpha = 0.8;
		ImGui_ImplGlfw_InitForOpenGL(window.Get(), true);
		ImGui_ImplOpenGL3_Init(nullptr);
		io.Fonts->AddFontDefault();
	}

	Menu::~Menu()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Menu::Render(Engine::Settings& settings) const
	{
		const static char* mssa[3] = {
			"none",
			"2x2",
			"4x4"
		};

		const static char* lightModels[2] = {
			"Phong-Blinn",
			"Oren-Nayar"
		};

		const static char* scenes[2] = {
			"Bunny",
			"Spheres"
		};

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

			ImGui::Begin("Menu", nullptr, flags);

			// ==================================

			ImGui::Text("CPU Rasterizer");
			ImGui::Separator();

			ImGui::Text("# spheres       %7i  ", settings.spheresCount);
			ImGui::Text("# triangles     %7i  ", settings.trianglesCount);
			ImGui::Text("# threads       %7i  ", std::thread::hardware_concurrency());
			ImGui::Text("# FPS           %7.1f", settings.fps);

			// ==================================
	
			ImGui::Text("Settings");
			ImGui::Separator();

			ImGui::Text("Scenes");

			ImGui::PushItemWidth(-1);
			ImGui::Combo("", &settings.sceneId, scenes, 2);
			ImGui::PopItemWidth();
			
			ImGui::Text("MSSA mode");

			ImGui::PushItemWidth(-1);
			ImGui::Combo(" ", &settings.MSSA, mssa, 3);
			ImGui::PopItemWidth();

			ImGui::Text("Light model");

			ImGui::PushItemWidth(-1);
			ImGui::Combo("  ", &settings.lightModelId, lightModels, 2);
			ImGui::PopItemWidth();

			ImGui::Checkbox("Use shadows", &settings.useShadows);
			
			// ==================================

			ImGui::Text("Controls");
			ImGui::Separator();
			ImGui::Text("W - forward \nD - right \nA - left \nS - back \n");
			ImGui::Text("Left mouse");

			ImGui::End();
		}
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}
