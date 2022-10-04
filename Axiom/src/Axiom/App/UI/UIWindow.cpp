#include "UIWindow.hpp"

void UIWindow::Construct(const UIWindow::WArguments& args)
{
	m_Type = args.m_Type;
	m_Title = args.m_Title;
	m_ScreenPosition = args.m_ScreenPosition;
	m_Size = args.m_Size;
	m_AutoCenter = args.m_AutoCenter;
	m_IsTopmostWindow = args.m_IsTopmostWindow;
	m_SupportsMinimize = args.m_SupportsMinimize;
	m_HasCloseButton = args.m_HasCloseButton;
}
