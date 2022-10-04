#include "GLFWWindow.hpp"
#include "GLFWEvents.hpp"

#include <iostream>

GLFWNativeWindow::~GLFWNativeWindow()
{
	glfwDestroyWindow(m_WindowHandle);
	m_WindowHandle = nullptr;
}

void GLFWNativeWindow::Initialize(const WindowInitDesc& windowInitDesc, const SharedPtr<INativeWindow>& parentWindow)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	glfwWindowHint(GLFW_DECORATED, windowInitDesc.IsDecorated ? GLFW_TRUE : GLFW_FALSE);

	if (windowInitDesc.Maximized && !windowInitDesc.FullScreen)
	{
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	}
	else
	{
		glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);
	}

	glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);

	GLFWmonitor* primary = glfwGetPrimaryMonitor();
	const GLFWvidmode* vidMode = glfwGetVideoMode(primary);

	if (windowInitDesc.FullScreen)
	{
		SetSize(vidMode->width, vidMode->height);
	}
	else
	{
		SetSize(windowInitDesc.DesiredSize);
	}

	m_WindowHandle = glfwCreateWindow(GetSize().x, GetSize().y, windowInitDesc.Title.CStr(), windowInitDesc.FullScreen ? primary : nullptr, nullptr);

	if(!m_WindowHandle) {
		std::cerr << "Cannot create window !" << std::endl;
		glfwTerminate();
		exit(1);
	}

	// Fix for linux not getting the right size by glfw event on fullscreen mode
	{
		int width, height;
		glfwGetWindowSize(m_WindowHandle, &width, &height);

		SetSize(width, height);
	}

	if(parentWindow != nullptr && !windowInitDesc.Maximized)
	{
		// TODO: Center relative to parent
	} else if(!windowInitDesc.Maximized && !windowInitDesc.FullScreen)
	{
		if (windowInitDesc.WindowLocation.IsSet())
		{
			int x = windowInitDesc.WindowLocation->x;
			int y = windowInitDesc.WindowLocation->y;
			glfwSetWindowPos(m_WindowHandle, x, y);
		}
		else
		{
			int x = vidMode->width / 2 - GetWidth() / 2;
			int y = vidMode->height / 2 - GetHeight() / 2;
			glfwSetWindowPos(m_WindowHandle, x, y);
		}
	}

	if(parentWindow != nullptr) {
#if _WIN32
		HWND hwNative = glfwGetWin32Window(m_WindowHandle);
		HWND parentHwNative = glfwGetWin32Window(((GLFWNativeWindow*)parentWindow.get())->GetWindowHandle());

		::SetParent(hwNative, parentHwNative);
#endif
	}

	glfwSetInputMode(m_WindowHandle, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
	glfwSetWindowUserPointer(m_WindowHandle, this);

	PlatformEvents::InitGLFWEventsForWindow(this);
}

void GLFWNativeWindow::Resize(int w, int h)
{
	glfwSetWindowSize(m_WindowHandle, w, h);
}

void GLFWNativeWindow::Resize(const Vector2i& size)
{
	Resize(size.x, size.y);
}

void GLFWNativeWindow::Show()
{
	if(m_WindowHandle != nullptr)
		glfwShowWindow(m_WindowHandle);

#if UNIX
	// If on linux you startup window maximized, it will not call the callback and instead of set wrong size, this should fix that.
	int width;
	int height;
	glfwGetWindowSize(m_WindowHandle, &width, &height);

	if(m_Size.x != width || m_Size.y != height)
	{
		OnResizeCallback(m_WindowHandle, width, height);
	}
#endif
}

void GLFWNativeWindow::Hide()
{
	if(m_WindowHandle != nullptr)
		glfwHideWindow(m_WindowHandle);
}

void GLFWNativeWindow::Destroy()
{
	if(m_WindowHandle != nullptr)
		glfwDestroyWindow(m_WindowHandle);
}

void GLFWNativeWindow::Minimize()
{
	if(m_WindowHandle != nullptr)
		glfwIconifyWindow(m_WindowHandle);
}

void GLFWNativeWindow::Maximize()
{
	if(m_WindowHandle != nullptr)
		glfwMaximizeWindow(m_WindowHandle);
}

void GLFWNativeWindow::Restore()
{
	if(m_WindowHandle != nullptr)
		glfwRestoreWindow(m_WindowHandle);
}

void GLFWNativeWindow::Focus()
{
	if(m_WindowHandle != nullptr)
		glfwFocusWindow(m_WindowHandle);
}

void GLFWNativeWindow::SetTitle(const String& title)
{
	glfwSetWindowTitle(m_WindowHandle, title.CStr());
}

bool GLFWNativeWindow::IsFocused() const
{
	return m_Focused;
}

bool GLFWNativeWindow::IsShouldClose() const
{
	if(m_WindowHandle == nullptr)
		return false;

	return glfwWindowShouldClose(m_WindowHandle);
}

void GLFWNativeWindow::SetCursorMode(const ECursorMode& mode)
{
	if(m_WindowHandle == nullptr)
		return;

	switch (mode)
	{
		case ECursorMode::Normal:
			glfwSetInputMode(m_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			break;
		case ECursorMode::Disabled:
			glfwSetInputMode(m_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			break;
		case ECursorMode::Hidden:
			glfwSetInputMode(m_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			break;
	}

	m_CursorMode = mode;
}

const ECursorMode& GLFWNativeWindow::GetCursorMode() const
{
	return m_CursorMode;
}

bool GLFWNativeWindow::IsIconified()
{
	return glfwGetWindowAttrib(m_WindowHandle, GLFW_ICONIFIED) == GLFW_TRUE;
}
