#include "Application.hpp"

using namespace std;

namespace sqrp
{
	void Application::CreateGLFWWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		pWindow_ = glfwCreateWindow(windowWidth_, windowHeight_, appName_.c_str(), nullptr, nullptr);
	}

	Application::Application(string appName, unsigned int windowWidth, unsigned int windowHeight)
		: appName_(appName), windowWidth_(windowWidth), windowHeight_(windowHeight)
	{
		CreateGLFWWindow();
	}

	void Application::Init()
	{

	}

	void Application::Render()
	{

	}

	void Application::Run()
	{
		while (!glfwWindowShouldClose(pWindow_)) {
			glfwPollEvents();

			Render();
		}
	}

	void Application::Terminate()
	{
		glfwDestroyWindow(pWindow_);
		glfwTerminate();
	}

	std::string Application::GetAppName()
	{
		return appName_;
	}

	GLFWwindow* Application::GetPWindow()
	{
		return pWindow_;
	}

	unsigned int Application::GetWindowWidth()
	{
		return windowWidth_;
	}

	unsigned int Application::GetWindowHeight()
	{
		return windowHeight_;
	}
}