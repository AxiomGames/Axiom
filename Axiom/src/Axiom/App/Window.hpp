#pragma once

#include "Axiom/CoreMinimal.hpp"

enum class EWindowMode
{
	Fullscreen,
	WindowedFullscreen,
	Windowed
};

// Not used rn
enum class EWindowType
{
	/** Value indicating that this is a standard, general-purpose window */
	Normal,

	/** Value indicating that this is a window used for a popup menu */
	Menu,
	/** Value indicating that this is a window used for a tooltip */
	ToolTip,
	/** Value indicating that this is a window used for a notification toast */
	Notification,
	/** Value indicating that this is a window used for a cursor decorator */
	CursorDecorator,
	/** Value indicating that this is a game window */
	GameWindow
};

enum class ECursorMode
{
	Normal = 0,
	Hidden,
	Disabled
};

struct WindowInitDesc
{
	Optional<EWindowType> WindowType;
	Vector2i DesiredSize;
	Optional<Vector2i> WindowLocation;
	String Title;

	bool IsDecorated = true;
	bool Maximized = false;
	bool FullScreen = false;

	bool SupportsMinimize = true;
	bool SupportsMaximize = true;
};


#ifdef AX_WIN32

// these are here because I didn't want to include whole windows.h
struct HWND__;
typedef HWND__* HWND;

#endif

class INativeWindow
{
private:
	int m_Width = 0;
	int m_Height = 0;
public:
	virtual void Initialize(const WindowInitDesc& windowInitDesc, const SharedPtr<INativeWindow>& parentWindow) = 0;

	virtual void Resize(int w, int h) = 0;
	virtual void Resize(const Vector2i& size) = 0;

	virtual void SetSize(int w, int h)
	{
		m_Width = w;
		m_Height = h;
	}
	virtual void SetSize(const Vector2i& size)
	{
		m_Width = size.x;
		m_Height = size.y;
	}

	virtual void Show() = 0;
	virtual void Hide() = 0;

	virtual void Destroy() = 0;

	virtual void Minimize() = 0;
	virtual void Maximize() = 0;
	virtual void Restore() = 0;

	virtual void Focus() = 0;

	virtual void SetTitle(const String& title) = 0;

	[[nodiscard]] virtual bool IsFocused() const = 0;
	[[nodiscard]] virtual bool IsShouldClose() const = 0;

	virtual void SetCursorMode(const ECursorMode& mode) = 0;
	[[nodiscard]] virtual const ECursorMode& GetCursorMode() const = 0;

	virtual bool IsIconified() = 0;

#ifdef AX_WIN32
	virtual HWND GetHWND() = 0;
#endif

	[[nodiscard]] FINLINE int GetWidth() const { return m_Width; }
	[[nodiscard]] FINLINE int GetHeight() const { return m_Height; }
	[[nodiscard]] Vector2i GetSize() const { return {m_Width, m_Height}; }
};
