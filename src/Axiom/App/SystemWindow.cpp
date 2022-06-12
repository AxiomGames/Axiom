#include "SystemWindow.hpp"
#include "Axiom/Graphics/RenderDevice.hpp"

#include <DXGI.h>
#include <D3D11.h>

namespace Ax
{
bool SystemWindow::GLFWInitialized = false;

void GLFWWindowSizeCallback(GLFWwindow* window, int w, int h)
{
	auto* systemWindow = static_cast<SystemWindow*>(glfwGetWindowUserPointer(window));
	systemWindow->NeedsUpdateSwapChain = true;
	systemWindow->Description.Width = w;
	systemWindow->Description.Height = h;
}

SystemWindow* SystemWindow::Create(const WindowDesc& desc, bool show, SystemWindow* parentWindow)
{
	if (!GLFWInitialized)
	{
		GLFWInitialized = true;
		glfwInit();
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	glfwWindowHint(GLFW_RESIZABLE, desc.Resizable ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	glfwWindowHint(GLFW_VISIBLE, show ? GLFW_TRUE : GLFW_FALSE);
	glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);

	glfwWindowHint(GLFW_MAXIMIZED, desc.Maximized ? GLFW_TRUE : GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(desc.Width, desc.Height, desc.Title.CStr(), nullptr, nullptr);

	if (window == nullptr)
	{
		// TODO: Throw error!
		return nullptr;
	}

	auto* systemWindow = new SystemWindow(window, desc);
	glfwSetWindowUserPointer(window, systemWindow);

	glfwSetWindowSizeCallback(window, GLFWWindowSizeCallback);

	return systemWindow;
}

void SystemWindow::UpdateSwapChain(RenderContext* renderContext)
{
	if (NeedsUpdateSwapChain)
	{
		NeedsUpdateSwapChain = false;

		if (BackBufferView != nullptr)
		{
			BackBufferView->Release();
		}

		if (BackBuffer != nullptr)
		{
			BackBuffer->Release();
		}

		SwapChain->ResizeBuffers(1, Description.Width, Description.Height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

		renderContext->CreateBackBuffer(this);
	}
}

void SystemWindow::Show()
{
	glfwShowWindow(Window);
}

void SystemWindow::Hide()
{
	glfwHideWindow(Window);
}

void SystemWindow::PoolEvents()
{
	glfwPollEvents();
}

void SystemWindow::SetTitle(const String& title)
{
	glfwSetWindowTitle(Window, title.CStr());
}

bool SystemWindow::ShouldClose() const
{
	return glfwWindowShouldClose(Window);
}

Vector2i SystemWindow::GetSize() const
{
	Vector2i vec;
	glfwGetWindowSize(Window, &vec.x, &vec.y);
	return vec;
}

SystemWindow::~SystemWindow()
{
	glfwDestroyWindow(Window);
}
}