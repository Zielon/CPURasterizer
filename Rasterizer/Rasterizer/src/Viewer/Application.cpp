#include "Application.h"

#include "Window.h"
#include "Menu.h"

namespace Viewer
{
	Application::Application()
	{
		window.reset(new Window(800, 800));
		menu.reset(new Menu());
	}

	Application::~Application() {}

	void Application::Run() const
	{
		while (!glfwWindowShouldClose(window->Get())) 
		{
			glClear(GL_DEPTH_BUFFER_BIT);
			glClear(GL_COLOR_BUFFER_BIT);
			
			menu->Render();
			
			glfwSwapBuffers(window->Get());
			glfwPollEvents();
		}
	}
}
