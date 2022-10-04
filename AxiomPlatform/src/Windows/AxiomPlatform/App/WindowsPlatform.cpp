#include "Axiom/App/PlatformBase.hpp"
#include "Axiom/App/WindowManager.hpp"
#include "Axiom/Engine.hpp"

#include "AxiomPlatform/App/GLFWWindow.hpp"
#include "AxiomPlatform/App/GLFWEvents.hpp"

namespace Platform
{
	bool Init()
	{
		bool glfwStatus = glfwInit() == GLFW_TRUE;
		PlatformEvents::InitGLFWEvents();

		return glfwStatus;
	}

	void Shutdown()
	{
		glfwTerminate();
	}

	void Update()
	{
		glfwPollEvents();

		if (GEngine->IsInitialized<WindowManager>())
		{
			GEngine->Get<WindowManager>()->UpdateWindows();
		}
	}

	SharedPtr<INativeWindow> MakeWindow()
	{
		return MakeShared<GLFWNativeWindow>();
	}
}