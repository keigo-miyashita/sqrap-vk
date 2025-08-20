#pragma once

#include "pch.hpp"

namespace sqrp
{
	class Application
	{
	private:

	protected:
		GLFWwindow* pWindow_;
		std::string appName_;
		unsigned int windowWidth_;
		unsigned int windowHeight_;

		void CreateGLFWWindow();
		virtual void Render();

	public:
		Application();
		Application(std::string appName = "App", unsigned int windowWidth = 1280, unsigned int windowHeight = 720);
		~Application() = default;

		virtual void Init();
		virtual void Run();
		virtual void Terminate();
		std::string GetAppName();
		GLFWwindow* GetPWindow();
		unsigned int GetWindowWidth();
		unsigned int GetWindowHeight();
	};
}