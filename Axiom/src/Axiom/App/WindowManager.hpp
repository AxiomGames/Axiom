#pragma once

#include "Axiom/Core/Common.hpp"
#include "UI/UIWindow.hpp"

class WindowManager
{
private:

public:
	SharedPtr<UIWindow> AddWindow(const SharedPtr<UIWindow>& window, bool showImmediately);
};
