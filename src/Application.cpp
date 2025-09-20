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

	Application::Application()
	{

	}

	Application::Application(string appName, unsigned int windowWidth, unsigned int windowHeight)
		: appName_(appName), windowWidth_(windowWidth), windowHeight_(windowHeight)
	{
		CreateGLFWWindow();
	}

	bool Application::Init()
	{
		OnStart();

		return true;
	}

	void Application::Run()
	{
		while (!glfwWindowShouldClose(pWindow_)) {
			glfwPollEvents();

			OnUpdate();
		}

		glfwDestroyWindow(pWindow_);
		glfwTerminate();
	}

	void Application::OnStart()
	{

	}

	void Application::OnUpdate()
	{

	}

	void Application::OnTerminate()
	{

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