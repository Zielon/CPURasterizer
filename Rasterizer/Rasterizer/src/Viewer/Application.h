#pragma once

#include <memory>

#include "../Engine/Settings.h"

namespace Engine
{
	class Renderer;
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
		void RegisterCallbacks();
		void MeasureTime();

		// OpenGL handlers
		unsigned int VBO{}, VAO{}, EBO{}, texture{};
		
		Engine::Settings settings{};
		
		std::unique_ptr<class Window> window;
		std::unique_ptr<class Camera> camera;
		std::unique_ptr<class Menu> menu;
		std::unique_ptr<class Shader> shader;
		std::unique_ptr<Engine::Renderer> renderer;
	};
}
