#pragma once

#include "Axiom/Core/String.hpp"
#include "Axiom/Math/Vector2.hpp"

#include <Windows.h>
#include <GLFW/glfw3.h>

class IDXGISwapChain;
class ID3D11Texture2D;
class ID3D11RenderTargetView;

AX_NAMESPACE

class SystemWindow;
class RenderContext;

struct WindowDesc
{
	int Width = 1280;
	int Height = 720;
	bool Fullscreen = false;
	bool Maximized = false;
	bool Resizable = true;
	String Title = "Unknown";
};

struct SystemWindow
{
	static bool GLFWInitialized;
	GLFWwindow* Window;
	WindowDesc Description;
	IDXGISwapChain* SwapChain = nullptr;
	ID3D11Texture2D* BackBuffer = nullptr;
	ID3D11RenderTargetView* BackBufferView = nullptr;
	bool NeedsUpdateSwapChain = true;

	SystemWindow(GLFWwindow* window, WindowDesc desc) : Window(window), Description(std::move(desc))
	{}

	~SystemWindow();

	static SystemWindow* Create(const WindowDesc& desc, bool show, SystemWindow* parentWindow = nullptr);

	void UpdateSwapChain(RenderContext* renderContext);

	void Show();

	void Hide();

	static void PoolEvents();

	void SetTitle(const String& title);

	[[nodiscard]] bool ShouldClose() const;

	[[nodiscard]] ax::Vector2i GetSize() const;
};

AX_END_NAMESPACE