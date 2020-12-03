#pragma once

#include <memory>

namespace Viewer
{
	class Application final
	{
	public:
		Application();
		~Application();
		
		void Run() const;

	private:
		std::unique_ptr<class Window> window;
	};
}
