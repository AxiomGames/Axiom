#pragma once

class GLFWNativeWindow;

namespace PlatformEvents
{
	void InitGLFWEvents();

	void InitGLFWEventsForWindow(GLFWNativeWindow* nativeWindow);
}