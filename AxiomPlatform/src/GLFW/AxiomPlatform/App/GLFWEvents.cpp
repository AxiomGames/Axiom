#include "GLFWEvents.hpp"
#include "GLFWWindow.hpp"

#include "Axiom/Core/Logger.hpp"

namespace PlatformEvents
{
	void JoystickCallback(int jid, int event)
	{
		//std::dynamic_pointer_cast<Input::Manager>(GEngine->GetWindow()->GetInputManager())->OnJoystickConnectChange(jid, event == GLFW_CONNECTED);

		if (event == GLFW_CONNECTED)
		{
			AX_LOG("The joystick was connected %d", jid);
		}
		else if (event == GLFW_DISCONNECTED)
		{
			AX_LOG("The joystick was disconnected %d", jid);
		}
	}

	void OnResizeCallback(GLFWwindow* rawWindow, int width, int height)
	{
		if(width == 0 || height == 0) return;

		auto* window = static_cast<GLFWNativeWindow*>(glfwGetWindowUserPointer(rawWindow));
		window->SetSize(width, height);
	}

	void OnFocusCallback(GLFWwindow* rawWindow, int focused)
	{
		auto* window = static_cast<GLFWNativeWindow*>(glfwGetWindowUserPointer(rawWindow));
		window->SetFocused(focused == GLFW_TRUE);
	}

	void OnKeyCallback(GLFWwindow* rawWindow, int key, int scancode, int action, int mods)
	{
		// TODO:
	}

	void OnCursorPosCallBack(GLFWwindow *rawWindow, double xpos, double ypos)
	{
		// TODO:
	}

	void OnMouseScrollCallback(GLFWwindow *rawWindow, double xoffset, double yoffset)
	{
		// TODO:
	}

	void OnMouseButtonCallback(GLFWwindow *rawWindow, int button, int action, int mods)
	{
		// TODO:
	}

	void CharModsCallback(GLFWwindow *rawWindow, uint32_t codepoint, int mods)
	{
		// TODO:
	}

	void OnFileDropListener(GLFWwindow* rawWindow, int count, const char** paths)
	{
		// TODO:
	}

	void InitGLFWEvents()
	{
		glfwSetJoystickCallback(JoystickCallback);
	}

	void InitGLFWEventsForWindow(GLFWNativeWindow* nativeWindow)
	{
		glfwSetWindowSizeCallback(nativeWindow->GetWindowHandle(), OnResizeCallback);
		glfwSetWindowFocusCallback(nativeWindow->GetWindowHandle(), OnFocusCallback);

		glfwSetKeyCallback(nativeWindow->GetWindowHandle(), OnKeyCallback);
		glfwSetCursorPosCallback(nativeWindow->GetWindowHandle(), OnCursorPosCallBack);
		glfwSetScrollCallback(nativeWindow->GetWindowHandle(), OnMouseScrollCallback);
		glfwSetMouseButtonCallback(nativeWindow->GetWindowHandle(), OnMouseButtonCallback);

		glfwSetCharModsCallback(nativeWindow->GetWindowHandle(), CharModsCallback);

		glfwSetDropCallback(nativeWindow->GetWindowHandle(), OnFileDropListener);
	}
}