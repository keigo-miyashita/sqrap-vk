#pragma once

#include "pch.hpp"

namespace sqrp
{
	struct MousePosition
	{
		int x;
		int y;
	};

	class Input
	{
	private:

		struct InputLogicalState
		{
			bool isRawPushed = false;
			bool isPushed = false;
		};

		static inline std::unordered_map<int, bool> isPushKey_ = {
			{GLFW_KEY_A, false},
			{GLFW_KEY_B, false},
			{GLFW_KEY_C, false},
			{GLFW_KEY_D, false},
			{GLFW_KEY_E, false},
			{GLFW_KEY_F, false},
			{GLFW_KEY_G, false},
			{GLFW_KEY_H, false},
			{GLFW_KEY_I, false},
			{GLFW_KEY_J, false},
			{GLFW_KEY_K, false},
			{GLFW_KEY_L, false},
			{GLFW_KEY_M, false},
			{GLFW_KEY_N, false},
			{GLFW_KEY_O, false},
			{GLFW_KEY_P, false},
			{GLFW_KEY_Q, false},
			{GLFW_KEY_R, false},
			{GLFW_KEY_S, false},
			{GLFW_KEY_T, false},
			{GLFW_KEY_U, false},
			{GLFW_KEY_V, false},
			{GLFW_KEY_W, false},
			{GLFW_KEY_X, false},
			{GLFW_KEY_Y, false},
			{GLFW_KEY_Z, false},
			{GLFW_KEY_SPACE, false},
			{GLFW_KEY_LEFT_CONTROL, false},
		};
		static inline std::unordered_map<int, InputLogicalState> isLogicalPushKey_ = {
			{GLFW_KEY_A, {false, false}},
			{GLFW_KEY_B, {false, false}},
			{GLFW_KEY_C, {false, false}},
			{GLFW_KEY_D, {false, false}},
			{GLFW_KEY_E, {false, false}},
			{GLFW_KEY_F, {false, false}},
			{GLFW_KEY_G, {false, false}},
			{GLFW_KEY_H, {false, false}},
			{GLFW_KEY_I, {false, false}},
			{GLFW_KEY_J, {false, false}},
			{GLFW_KEY_K, {false, false}},
			{GLFW_KEY_L, {false, false}},
			{GLFW_KEY_M, {false, false}},
			{GLFW_KEY_N, {false, false}},
			{GLFW_KEY_O, {false, false}},
			{GLFW_KEY_P, {false, false}},
			{GLFW_KEY_Q, {false, false}},
			{GLFW_KEY_R, {false, false}},
			{GLFW_KEY_S, {false, false}},
			{GLFW_KEY_T, {false, false}},
			{GLFW_KEY_U, {false, false}},
			{GLFW_KEY_V, {false, false}},
			{GLFW_KEY_W, {false, false}},
			{GLFW_KEY_X, {false, false}},
			{GLFW_KEY_Y, {false, false}},
			{GLFW_KEY_Z, {false, false}},
			{GLFW_KEY_SPACE, {false, false}},
			{GLFW_KEY_LEFT_CONTROL, {false, false}},
		};
		static inline int wheel_ = 0;
		static inline bool isPushedLButton_ = false;
		static inline bool isPushedRButton_ = false;
		static inline MousePosition pushedMousePos_;
		static inline MousePosition prevMousePos_;
		static inline MousePosition currentMousePos_;
		static inline MousePosition deltaMousePos_;

	public:
		//static void GetRawState(UINT msg, WPARAM wparam, LPARAM lparam);
		static void Update(double x, double y);
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void WindowFocusCallback(GLFWwindow* window, int focused);
		static bool IsPushKey(int key);
		static int GetWheel();
		static MousePosition GetPushedPos();
		static MousePosition GetPrevPos();
		static MousePosition GetDeltaPos();
		static MousePosition GetPos();
		static bool IsPushedLButton();
		static bool IsPushedRButton();
	};

	class Application
	{
	private:

	protected:
		GLFWwindow* pWindow_;
		std::string appName_;
		unsigned int windowWidth_;
		unsigned int windowHeight_;

		static void WindowSizeCallback(GLFWwindow* window, int width, int height);

		void CreateGLFWWindow();

	public:
		Application();
		Application(std::string appName = "App", unsigned int windowWidth = 1280, unsigned int windowHeight = 720);
		~Application() = default;

		bool Init();
		void Run();
		virtual void OnStart();
		virtual void OnUpdate();
		virtual void OnResize(unsigned int width, unsigned int height);
		virtual void OnTerminate();
		std::string GetAppName();
		GLFWwindow* GetPWindow();
		unsigned int GetWindowWidth();
		unsigned int GetWindowHeight();

		void SetWindowWidth(unsigned int width);
		void SetWindowHeight(unsigned int height);
	};
}