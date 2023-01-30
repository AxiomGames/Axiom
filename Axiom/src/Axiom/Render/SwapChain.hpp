#pragma once
#include "Types.hpp"

struct SwapChainDesc
{
	int width = 0, height = 0;
	EImageFormat imageFormat = EImageFormat::UNKNOWN;
};

struct ISwapChain
{
	virtual void Present() = 0;
};