#pragma once

#include <iostream>
#include "Axiom/CoreMinimal.hpp"

#ifdef AX_WIN32

#include <dxgi1_6.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <wrl.h>

using namespace Microsoft::WRL;

#define AX_DEBUG_BREAK DebugBreak()

#ifndef DXCall
	#ifdef _DEBUG
#define DXCall(x) \
	if (FAILED(x))    \
	{                 \
		printf("Error in: %s \nLine: %d \n %s\n", __FILE__, __LINE__, #x);\
		AX_DEBUG_BREAK;\
	}
	#else
#define DXCall(x)
	#endif
#endif

template<typename T>
constexpr void ReleaseResource(T*& resource)
{
	if (resource)
	{
		resource->Release();
		resource = nullptr;
	}
}

#endif