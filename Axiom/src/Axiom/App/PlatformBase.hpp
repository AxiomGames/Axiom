#pragma once

#include "Axiom/Core/SharedPtr.hpp"

class INativeWindow;

namespace Platform
{
	bool Init();
	void Shutdown();
	void Update();
	SharedPtr<INativeWindow> MakeWindow();
}