#include "Application.h"

#include "Window.h"
#include "Menu.h"
#include "Shader.h"

#include "../Engine/Renderer.h"
#include "../Engine/Camera.h"
#include "../Engine/Scene.h"

#include "../path.h"

float lastTime = 0;
long int currentFrame = 0;

namespace Viewer
{
	static const inline std::vector<std::string> CONFIGS =
	{
		"ajax.scene",
		"bedroom.scene",
		"boy.scene",
		"coffee_cart.scene",
		"coffee_maker.scene",
		"cornell_box.scene",
		"diningroom.scene",
		"dragon.scene",
		"hyperion.scene",
		"panther.scene",
		"spaceship.scene",
		"staircase.scene",
		"stormtrooper.scene",
		"teapot.scene",
		"hyperion2.scene",
		"mustang.scene",
		"mustang_red.scene",
		"furnace.scene"
	};

	static std::string GetScenePath(uint32_t index)
	{
		auto root = Path::Root({ "Rasterizer", "Assets", "PBRScenes" });
		const auto path = root / CONFIGS[index];
		std::cout << "[SCENE] Scene selected " << path.string() << std::endl;
		return path.string();
	}

	Application::Application()
	{
		CreateRender();
		CreateWindow();
		CreatePipeline();

		RegisterCallbacks();

		menu->GetSettings().trianglesCount = scene->GetIndexSize() / 3;
		menu->GetSettings().resolution = scene->GetRendererOptions().resolution;
	}

	Application::~Application() = default;

	void Application::CreateWindow()
	{
		// Viewer composition
		window.reset(new Window(scene->GetCamera().GetWidth(), scene->GetCamera().GetHeight()));
		menu.reset(new Menu(*window));
		shader.reset(new Shader("Quad.vert", "Quad.frag"));
	}

	void Application::CreateRender()
	{
		// Renderer composition
		scene.reset(new Engine::Scene(GetScenePath(settings.sceneId)));
		renderer.reset(new Engine::Renderer(*scene, scene->GetCamera()));
	}

	void Application::Run()
	{
		lastTime = glfwGetTime();

		while (!glfwWindowShouldClose(window->Get()))
		{
			Engine::Camera::TimeDeltaUpdate();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			scene->GetCamera().OnBeforeRender();
			renderer->Render(menu->GetSettings());
			DrawQuad();
			menu->Render();

			glfwSwapBuffers(window->Get());
			glfwPollEvents();

			MeasureTime();
			UpdateSettings();
		}
	}

	void Application::DrawQuad() const
	{
		// Copy frameBuffer to texture
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA, scene->GetCamera().GetWidth(), scene->GetCamera().GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
			renderer->GetColorBuffer());

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

			if (scene->GetCamera().OnCursorPositionChanged(xpos, ypos)) {}
		});

		window->AddOnKeyChanged([this](const int key, const int scancode, const int action, const int mods)-> void
		{
			if (menu->WantCaptureKeyboard())
				return;

			scene->GetCamera().OnKeyChanged(key, scancode, action, mods);
		});

		window->AddOnMouseButtonChanged([this](const int button, const int action, const int mods)-> void
		{
			if (menu->WantCaptureMouse())
				return;

			scene->GetCamera().OnMouseButtonChanged(button, action, mods);
		});

		window->AddOnScrollChanged([this](const double xoffset, const double yoffset)-> void { });
	}

	void Application::MeasureTime() const
	{
		currentFrame++;

		const float currentTime = glfwGetTime();
		const float delta = currentTime - lastTime;

		if (delta > 1.0)
		{
			const double fFPS = currentFrame / delta;
			lastTime = currentTime;
			currentFrame = 0L;
			menu->GetSettings().fps = static_cast<float>(fFPS);
		}
	}

	void Application::Resize() const
	{
		const auto width = scene->GetCamera().GetWidth();
		const auto height = scene->GetCamera().GetHeight();
		glfwSetWindowSize(window->Get(), width, height);
		glViewport(0, 0, width, height);
	}

	void Application::UpdateSettings()
	{
		if (settings.sceneId != menu->GetSettings().sceneId)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			settings = menu->GetSettings();
			Recreate();
			menu->GetSettings().trianglesCount = scene->GetIndexSize() / 3;
			menu->GetSettings().resolution = scene->GetRendererOptions().resolution;
		}
	}

	void Application::Recreate()
	{
		CreateRender();
		Resize();
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
