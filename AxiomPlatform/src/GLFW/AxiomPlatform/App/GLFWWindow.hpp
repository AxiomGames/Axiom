#pragma once

#include "Axiom/App/Window.hpp"

#ifdef AX_WIN32

// these are here because I didn't want to include whole windows.h
struct HWND__;
typedef HWND__* HWND;

#endif

struct GLFWwindow;

class GLFWNativeWindow : public INativeWindow
{
private:
	GLFWwindow* m_WindowHandle = nullptr;

	ECursorMode m_CursorMode = ECursorMode::Normal;
	bool m_Focused = false;
public:
	~GLFWNativeWindow();

	void Initialize(const WindowInitDesc& windowInitDesc, const SharedPtr<INativeWindow>& parentWindow) override;

	void Resize(int w, int h) override;

	void Resize(const Vector2i& size) override;

	void Show() override;

	void Hide() override;

	void Destroy() override;

	void Minimize() override;

	void Maximize() override;

	void Restore() override;

	void Focus() override;

	void SetTitle(const String& title) override;

#ifdef AX_WIN32
	HWND GetHWND();
#endif

	[[nodiscard]] bool IsFocused() const override;

	[[nodiscard]] bool IsShouldClose() const override;

	void SetCursorMode(const ECursorMode& mode) override;

	[[nodiscard]] const ECursorMode& GetCursorMode() const override;

	bool IsIconified() override;

	void SetFocused(bool focused) { m_Focused = focused; }

	[[nodiscard]] GLFWwindow* GetWindowHandle() const { return m_WindowHandle; }
};