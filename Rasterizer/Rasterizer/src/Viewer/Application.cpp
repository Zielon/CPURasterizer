#include "Application.h"

#include <glm/glm.hpp>

#include "Window.h"
#include "Menu.h"
#include "Camera.h"

#include "../Engine/Renderer.h"

namespace Viewer
{
	Application::Application()
	{
		window.reset(new Window(1024, 1024));
		menu.reset(new Menu(*window));
		renderer.reset(new Engine::Renderer());
		camera.reset(new Camera({}, {}, 0, 0));

		RegisterCallbacks();
	}

	Application::~Application() {}

	void Application::Run()
	{
		while (!glfwWindowShouldClose(window->Get()))
		{
			glClear(GL_DEPTH_BUFFER_BIT);
			glClear(GL_COLOR_BUFFER_BIT);

			menu->Render(settings);
			renderer->Render(settings);

			glfwSwapBuffers(window->Get());
			glfwPollEvents();
		}
	}

	void Application::RegisterCallbacks()
	{
		window->AddOnCursorPositionChanged([this](const double xpos, const double ypos)-> void
		{
			if (menu->WantCaptureKeyboard() || menu->WantCaptureMouse())
				return;

			if (camera->OnCursorPositionChanged(xpos, ypos)) {}
		});

		window->AddOnKeyChanged([this](const int key, const int scancode, const int action, const int mods)-> void
		{
			if (menu->WantCaptureKeyboard())
				return;

			camera->OnKeyChanged(key, scancode, action, mods);
		});

		window->AddOnMouseButtonChanged([this](const int button, const int action, const int mods)-> void
		{
			if (menu->WantCaptureMouse())
				return;

			camera->OnMouseButtonChanged(button, action, mods);
		});

		window->AddOnScrollChanged([this](const double xoffset, const double yoffset)-> void { });
	}
}
