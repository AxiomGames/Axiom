#include "UIWindow.hpp"

void UIWindow::Construct(const UIWindow::WArguments& args)
{
	m_Type = args.m_Type;
	m_Title = args.m_Title;
	m_ScreenPosition = args.m_ScreenPosition;
	m_Size = args.m_Size;

	m_Decorated = args.m_Decorated;
	m_AutoCenter = args.m_AutoCenter;
	m_IsTopmostWindow = args.m_TopmostWindow;
	m_SupportsMinimize = args.m_SupportsMinimize;
	m_SupportsMaximize = args.m_SupportsMaximize;
	m_HasCloseButton = args.m_HasCloseButton;
	m_IsFullScreen = args.m_FullScreen;
	m_Maximized = args.m_Maximized;
}

void UIWindow::Show()
{
	if (m_NativeWindow)
		m_NativeWindow->Show();
}

void UIWindow::Hide()
{
	if (m_NativeWindow)
		m_NativeWindow->Hide();
}

void UIWindow::SetFocus()
{
	if (m_NativeWindow)
		m_NativeWindow->Focus();
}
