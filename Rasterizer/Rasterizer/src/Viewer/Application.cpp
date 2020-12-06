#include "Application.h"

#include <glm/glm.hpp>

#include "Window.h"
#include "Menu.h"
#include "Shader.h"

#include "../Engine/Renderer.h"
#include "../Engine/Camera.h"
#include "../Engine/Scene.h"

float lastTime = 0;
int nbFrames = 0;

namespace Viewer
{
	Application::Application()
	{
		// Renderer composition
		scene.reset(new Engine::Scene());
		camera.reset(new Engine::Camera({ .276f, .275f, - .75f }, { .276f, .275f, .10 }, 40, 1.f));
		renderer.reset(new Engine::Renderer(*scene, *camera));

		// Viewer composition
		window.reset(new Window(WIDTH, HEIGHT));
		menu.reset(new Menu(*window));
		shader.reset(new Shader("Quad.vert", "Quad.frag"));

		CreatePipeline();
		RegisterCallbacks();

		settings.trianglesCount = scene->GetTriangleCount();
	}

	Application::~Application() = default;

	void Application::Run()
	{
		lastTime = glfwGetTime();

		while (!glfwWindowShouldClose(window->Get()))
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			camera->OnBeforeRender();

			renderer->Render(settings);

			DrawQuad();

			menu->Render(settings);

			glfwSwapBuffers(window->Get());
			glfwPollEvents();

			MeasureTime();
		}
	}

	void Application::DrawQuad() const
	{
		// Copy framebuffer to texture
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, renderer->GetFrameBuffer());

		glBindTexture(GL_TEXTURE_2D, texture);

		shader->Use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
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

	void Application::MeasureTime()
	{
		float currentTime = glfwGetTime();
		float delta = currentTime - lastTime;

		nbFrames++;
		if (delta >= 1.f)
		{
			settings.fpms = 1000.f / nbFrames;
			nbFrames = 0;
			lastTime += 1.f;
		}
	}

	void Application::CreatePipeline()
	{
		static float vertices[] =
		{
			1.f, 1.f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
			1.f, -1.f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
			-1.f, -1.f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
			-1.f, 1.f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left 
		};

		static unsigned int indices[] =
		{
			0, 1, 3,
			1, 2, 3
		};

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}
