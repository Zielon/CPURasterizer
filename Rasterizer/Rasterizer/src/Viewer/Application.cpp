#include "Application.h"

#include "Window.h"

namespace Viewer
{
	Application::Application()
	{
		window.reset(new Window(800, 800));
	}

	Application::~Application() {}

	void Application::Run() const
	{
		while (!glfwWindowShouldClose(window->Get())) 
		{
			glfwPollEvents();
		}
	}
}
