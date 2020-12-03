#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Viewer
{
	class Window final
	{
	public:
		Window(int width, int height);
		~Window();

		[[nodiscard]] GLFWwindow* Get() const;

	private:
		GLFWwindow* window{ nullptr };
		const int width{};
		const int height{};
	};
}
