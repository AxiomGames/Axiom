#pragma once

#include "Axiom/CoreMinimal.hpp"
#include "C:/Users/Administrator/source/repos/Axiom/AxiomPlatform/src/GLFW/AxiomPlatform/App/GLFWWindow.hpp"

#ifdef AX_WIN32

namespace DX12
{
	void Initialize(SharedPtr<GLFWNativeWindow> window);
	void Shutdown();
	void Render();
}

#endif