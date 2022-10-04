#pragma once

#include "Widget.hpp"
#include "../Window.hpp"

class UIWindow : public Widget
{
	BEGIN_WIDGET_ARGS(UIWindow)
		: m_Type(EWindowType::Normal)
		, m_Title()
		, m_AutoCenter(true)
		, m_ScreenPosition()
		, m_IsTopmostWindow(false)
		, m_SupportsMaximize(true)
		, m_SupportsMinimize(true)
		, m_HasCloseButton(true)
		{}
		WIDGET_ATTRIBUTE(EWindowType, Type)
		WIDGET_ATTRIBUTE(String, Title)

		WIDGET_ATTRIBUTE(Vector2i, ScreenPosition)
		WIDGET_ATTRIBUTE(Vector2i, Size)

		WIDGET_ATTRIBUTE(bool, AutoCenter)
		WIDGET_ATTRIBUTE(bool, IsTopmostWindow)
		WIDGET_ATTRIBUTE(bool, SupportsMinimize)
		WIDGET_ATTRIBUTE(bool, SupportsMaximize)
		WIDGET_ATTRIBUTE(bool, HasCloseButton)
	END_WIDGET_ARGS
private:
	SharedPtr<INativeWindow> m_NativeWindow = nullptr;

	EWindowType m_Type;
	String m_Title;
	Vector2i m_ScreenPosition;
	Vector2i m_Size;

	bool m_AutoCenter;
	bool m_IsTopmostWindow;
	bool m_SupportsMinimize;
	bool m_HasCloseButton;
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
};