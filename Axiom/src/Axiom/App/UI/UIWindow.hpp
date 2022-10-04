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
		{}
		WIDGET_ATTRIBUTE(EWindowType, Type)
		WIDGET_ATTRIBUTE(String, Title)
		WIDGET_ATTRIBUTE(bool, AutoCenter)
		WIDGET_ATTRIBUTE(Vector2i, ScreenPosition)
		WIDGET_ATTRIBUTE(Vector2i, Size)
	END_WIDGET_ARGS
private:
	SharedPtr<INativeWindow> m_NativeWindow = nullptr;
public:

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