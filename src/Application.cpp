#include "Application.hpp"

using namespace std;

namespace sqrp
{
	void Input::Update(double x, double y, bool isCatchInput)
	{
		for (auto& [vk, rawState] : isPushKey_) {
			bool prev = isLogicalPushKey_[vk].isRawPushed;
			isLogicalPushKey_[vk].isRawPushed = rawState;

			if (!prev && rawState) {
				isLogicalPushKey_[vk].isPushed = true;
				cout << "Key Pushed: " << vk << endl;
			}
			if (prev && !rawState) {
				isLogicalPushKey_[vk].isPushed = false;
			}
		}
		currentMousePos_.x = x;
		currentMousePos_.y = y;

		deltaMousePos_.x = currentMousePos_.x - prevMousePos_.x;
		deltaMousePos_.y = currentMousePos_.y - prevMousePos_.y;

		prevMousePos_ = currentMousePos_;
	}

	void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		/*if (ImGui::GetCurrentContext() != nullptr) {
			ImGuiIO& io = ImGui::GetIO();
			if (io.WantCaptureKeyboard) {
				isPushedLButton_ = false;
				isPushedRButton_ = false;
				for (auto& [vk, rawState] : isPushKey_) {
					rawState = false;
				}
				return;
			}
		}*/
		switch (action) {
			case GLFW_PRESS:
				isPushKey_[key] = true;
				break;
			case GLFW_RELEASE:
				isPushKey_[key] = false;
				break;
		}
	}

	void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		/*if (ImGui::GetCurrentContext() != nullptr) {
			ImGuiIO& io = ImGui::GetIO();
			if (io.WantCaptureMouse) {
				isPushedLButton_ = false;
				isPushedRButton_ = false;
				for (auto& [vk, rawState] : isPushKey_) {
					rawState = false;
				}
				return;
			}
		}*/
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (action == GLFW_PRESS) {
				isPushedLButton_ = true;
			}
			else if (action == GLFW_RELEASE) {
				isPushedLButton_ = false;
			}
		}
	}

	void Input::WindowFocusCallback(GLFWwindow* window, int focused)
	{
		if (!focused) {
			isPushedLButton_ = false;
			isPushedRButton_ = false;
			for (auto& [vk, rawState] : isPushKey_) {
				rawState = false;
			}
		}
	}

	bool Input::IsPushKey(int key)
	{
		if (!isCatch_) {
			return false;
		}
		return isLogicalPushKey_[key].isPushed;
	}

	int Input::GetWheel()
	{
		if (!isCatch_) {
			return 0;
		}
		return wheel_;
	}

	MousePosition Input::GetPushedPos()
	{
		return pushedMousePos_;
	}

	MousePosition Input::GetPrevPos()
	{
		return prevMousePos_;
	}

	MousePosition Input::GetDeltaPos()
	{
		if (!isCatch_) {
			return { 0, 0 };
		}
		return deltaMousePos_;
	}

	MousePosition Input::GetPos()
	{
		return currentMousePos_;
	}

	bool Input::IsPushedLButton()
	{
		return isPushedLButton_;
	}

	bool Input::IsPushedRButton()
	{
		return isPushedRButton_;
	}

	void Input::SetCatchInput(bool isCatch)
	{
		isCatch_ = isCatch;
	}

	void Application::WindowSizeCallback(GLFWwindow* window, int width, int height)
	{
		Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
		app->SetWindowWidth(width);
		app->SetWindowHeight(height);
		app->OnResize(width, height);
	}

	void Application::CreateGLFWWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
		pWindow_ = glfwCreateWindow(windowWidth_, windowHeight_, appName_.c_str(), nullptr, nullptr);

		int fbWidth, fbHeight;
		glfwGetFramebufferSize(pWindow_, &fbWidth, &fbHeight);
		std::cout << "Before Adjusted framebuffer size: " << fbWidth << " x " << fbHeight << std::endl;

		windowWidth_ = fbWidth;
		windowHeight_ = fbHeight;

		glfwSetWindowUserPointer(pWindow_, this);

		glfwSetKeyCallback(pWindow_, Input::KeyCallback);
		glfwSetMouseButtonCallback(pWindow_, Input::MouseButtonCallback);
		glfwSetWindowFocusCallback(pWindow_, Input::WindowFocusCallback);
		glfwSetWindowSizeCallback(pWindow_, WindowSizeCallback);
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

			double x, y;
			glfwGetCursorPos(pWindow_, &x, &y);

			Input::Update(x, y);
			OnUpdate();
		}

		OnTerminate();
		glfwDestroyWindow(pWindow_);
		glfwTerminate();
	}

	void Application::OnStart()
	{

	}

	void Application::OnUpdate()
	{

	}

	void Application::OnResize(unsigned int width, unsigned int height)
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

	void Application::SetWindowWidth(unsigned int width)
	{
		windowWidth_ = width;
	}

	void Application::SetWindowHeight(unsigned int height)
	{
		windowHeight_ = height;
	}
}