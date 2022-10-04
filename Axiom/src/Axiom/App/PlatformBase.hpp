#pragma once

#include "Axiom/Core/Common.hpp"

class INativeWindow;

namespace Platform
{
	bool Init();
	void Shutdown();
	void Update();
	SharedPtr<INativeWindow> MakeWindow();
}