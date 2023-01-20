#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
#endif

#include <GLFW/glfw3.h>

#ifdef _WIN32
	#define GLFW_EXPOSE_NATIVE_WIN32
	#include <GLFW/glfw3native.h>
#endif