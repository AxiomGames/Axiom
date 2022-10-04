#pragma once

#include "Axiom/Core/Common.hpp"
#include "Axiom/Core/Array.hpp"
#include "UI/UIWindow.hpp"

class WindowManager
{
private:
	Array<SharedPtr<UIWindow>> m_Windows;
public:
	SharedPtr<UIWindow> AddWindow(const SharedPtr<UIWindow>& window, bool showImmediately);
	SharedPtr<UIWindow> FindUIWindowByNativeWindow(SharedPtr<INativeWindow>& nativeWindow);
	void UpdateWindows();

	static void MakeWindow(const SharedPtr<UIWindow>& window, const SharedPtr<UIWindow>& parentWindow = nullptr);
};
