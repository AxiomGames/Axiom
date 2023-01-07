#include "WindowManager.hpp"
#include "PlatformBase.hpp"

SharedPtr<UIWindow> WindowManager::AddWindow(const SharedPtr<UIWindow>& window, bool showImmediately)
{
	m_Windows.Add(window);

	MakeWindow(window);

	if (showImmediately)
	{
		window->Show();

		if (window->SupportsKeyboardFocus() && window->IsFocusedInitially())
		{
			window->SetFocus();
		}
	}

	return window;
}

SharedPtr<UIWindow> WindowManager::FindUIWindowByNativeWindow(SharedPtr<INativeWindow>& nativeWindow)
{
	for (const SharedPtr<UIWindow>& window : m_Windows)
	{
		if (window->GetNativeWindow() == nativeWindow)
		{
			return window;
		}
	}

	return nullptr;
}

void WindowManager::MakeWindow(const SharedPtr<UIWindow>& window, const SharedPtr<UIWindow>& parentWindow)
{
	WindowInitDesc windowInitDesc;
	windowInitDesc.WindowType = EWindowType::Normal;
	windowInitDesc.Title = window->GetTitle();
	windowInitDesc.WindowLocation = window->GetScreenPosition();
	windowInitDesc.IsDecorated = window->IsDecorated();
	windowInitDesc.FullScreen = window->IsFullScreen();
	windowInitDesc.DesiredSize = window->GetSize();
	windowInitDesc.Maximized = window->IsMaximized();
	windowInitDesc.SupportsMinimize = window->DoesSupportMinimize();
	windowInitDesc.SupportsMaximize = window->DoesSupportMaximize();

	SharedPtr<INativeWindow> nativeWindow = Platform::MakeWindow();
	nativeWindow->Initialize(windowInitDesc, parentWindow != nullptr ? parentWindow->GetNativeWindow() : nullptr);
	window->SetNativeWindow(nativeWindow);
}

void WindowManager::UpdateWindows()
{
	for (const SharedPtr<UIWindow>& window : m_Windows)
	{
		const SharedPtr<INativeWindow>& nativeWindow = window->GetNativeWindow();

		if (nativeWindow->IsShouldClose())
		{

		}
	}
}


