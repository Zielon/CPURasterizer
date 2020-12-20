#pragma once

#include <memory>

#include "../Engine/Settings.h"

namespace Engine
{
	class Renderer;
	class Camera;
	class Scene;
}

namespace Viewer
{
	class Application final
	{
	public:
		Application();
		~Application();

		void Run();

	private:
		void DrawQuad() const;
		void CreatePipeline();
		void CreateRender();
		void CreateWindow();
		void RegisterCallbacks();
		void MeasureTime() const;
		void Resize() const;
		void UpdateSettings();
		void Recreate();

		// OpenGL handlers
		unsigned int VBO{}, VAO{}, EBO{}, texture{};

		Engine::Settings settings{};

		// Viewer
		std::unique_ptr<class Window> window;
		std::unique_ptr<class Menu> menu;
		std::unique_ptr<class Shader> shader;

		// Engine
		std::unique_ptr<Engine::Renderer> renderer;
		std::unique_ptr<Engine::Scene> scene;
	};
}
