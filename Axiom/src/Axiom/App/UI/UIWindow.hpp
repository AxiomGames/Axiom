#pragma once

#include "Widget.hpp"
#include "../Window.hpp"

class UIWindow : public Widget
{
	BEGIN_WIDGET_ARGS(UIWindow)
		: m_Type(EWindowType::Normal)
		, m_Title()
		, m_Decorated(true)
		, m_AutoCenter(true)
		, m_ScreenPosition()
		, m_TopmostWindow(false)
		, m_SupportsMaximize(true)
		, m_SupportsMinimize(true)
		, m_HasCloseButton(true)
		, m_FullScreen(false)
		, m_Maximized(false)
		{}
		WIDGET_ATTRIBUTE(EWindowType, Type)
		WIDGET_ATTRIBUTE(String, Title)

		WIDGET_ATTRIBUTE(Optional<Vector2i>, ScreenPosition)
		WIDGET_ATTRIBUTE(Vector2i, Size)

		WIDGET_ATTRIBUTE(bool, Decorated)
		WIDGET_ATTRIBUTE(bool, AutoCenter)
		WIDGET_ATTRIBUTE(bool, TopmostWindow)
		WIDGET_ATTRIBUTE(bool, SupportsMinimize)
		WIDGET_ATTRIBUTE(bool, SupportsMaximize)
		WIDGET_ATTRIBUTE(bool, HasCloseButton)
		WIDGET_ATTRIBUTE(bool, FullScreen)
		WIDGET_ATTRIBUTE(bool, Maximized)
	END_WIDGET_ARGS
private:
	SharedPtr<INativeWindow> m_NativeWindow = nullptr;

	EWindowType m_Type;
	String m_Title;
	Optional<Vector2i> m_ScreenPosition;
	Vector2i m_Size;

	bool m_Decorated;
	bool m_AutoCenter;
	bool m_IsTopmostWindow;
	bool m_SupportsMinimize;
	bool m_SupportsMaximize;
	bool m_HasCloseButton;
	bool m_IsFullScreen;
	bool m_Maximized;
public:
	void Construct(const WArguments& args);

	const SharedPtr<INativeWindow>& GetNativeWindow() const
	{ return m_NativeWindow; }

	void SetNativeWindow(const SharedPtr<INativeWindow>& nativeWindow)
	{ m_NativeWindow = nativeWindow; }
private:
	LayerID OnPaint() const override
	{
		return 0;
	}
public:
	void Show();
	void Hide();

	void SetFocus();

	bool IsFocusedInitially() const { return true; }
	bool SupportsKeyboardFocus() const { return true; }
public:
	const EWindowType& GetType() const { return m_Type; }
	const String& GetTitle() const { return m_Title; }
	const Optional<Vector2i>& GetScreenPosition() const { return m_ScreenPosition; }
	const Vector2i& GetSize() const { return m_Size; }
	bool IsDecorated() const { return m_Decorated; }
	bool IsAutoCenter() const { return m_AutoCenter; }
	bool IsTopMostWindow() const { return m_IsTopmostWindow; }
	bool DoesSupportMinimize() const { return m_SupportsMinimize; }
	bool DoesSupportMaximize() const { return m_SupportsMaximize; }
	bool HasCloseButton() const { return m_HasCloseButton; }
	bool IsFullScreen() const { return m_IsFullScreen; }
	bool IsMaximized() const { return m_Maximized; }
};