#pragma once

#include "Axiom/CoreMinimal.hpp"

#ifdef AX_WIN

namespace DX12
{
	void Initialize();
	void Shutdown();

	void Render();
}

#endif