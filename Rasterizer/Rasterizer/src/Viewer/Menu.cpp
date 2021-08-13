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

	void Menu::Render()
	{
		const static uint32_t cores = std::thread::hardware_concurrency();
		const static char* filter[3] = { "Nearest", "Linear", "Trilinear" };
		const static char* lightModels[2] = { "Normals", "Phong-Blinn" };
		const static char* scenes[18] = {
			"Ajax",
			"Bedroom",
			"Boy",
			"Coffee cart",
			"Coffee maker",
			"Cornell box",
			"Dining room",
			"Dragon",
			"Hyperion",
			"Panther",
			"Spaceship",
			"Staircase",
			"Stormtroopers",
			"Teapot",
			"Hyperion II",
			"Mustang",
			"Mustang Red",
			"Furnace"
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

			ImGui::Text("Resolution %4ix%4i   ", settings.resolution.x, settings.resolution.y);
			ImGui::Text("# spheres       %7i  ", settings.spheresCount);
			ImGui::Text("# triangles     %7i  ", settings.trianglesCount);
			ImGui::Text("# threads       %7i  ", cores);
			ImGui::Text("# frames [s]    %7.1f", settings.fps);

			// ==================================

			ImGui::Text("Settings");
			ImGui::Separator();

			ImGui::Text("Scenes");

			ImGui::PushItemWidth(-1);
			ImGui::Combo("", &settings.sceneId, scenes, 18);
			ImGui::PopItemWidth();

			ImGui::Text("Fragment shader");

			ImGui::PushItemWidth(-1);
			ImGui::Combo("  ", &settings.lightModelId, lightModels, 2);
			ImGui::PopItemWidth();

			ImGui::Text("Texture filter");

			ImGui::PushItemWidth(-1);
			ImGui::Combo("   ", &settings.textureFilterId, filter, 3);
			ImGui::PopItemWidth();

			//ImGui::Checkbox("FXAA", &settings.FXAA);
			ImGui::Checkbox("Cull back-faces", &settings.cullBackFaces);
			//ImGui::Checkbox("Use shadows", &settings.useShadows);
			ImGui::Checkbox("Use Gamma Correction", &settings.useGammaCorrection);

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
