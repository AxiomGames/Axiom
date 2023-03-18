#pragma once

#include <iostream>
#include "Axiom/CoreMinimal.hpp"

#ifdef AX_WIN32

#include <dxgi1_6.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <wrl.h>

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

static constexpr uint32 g_NumBackBuffers = 2;

template<typename T>
constexpr void ReleaseResource(T*& resource)
{
	if (resource)
	{
		resource->Release();
		resource = nullptr;
	}
}

template<class... Args>
static inline void D3D12SetName(ID3D12Object* obj, const char* name, Args&&... args)
{
	wchar_t buffer[1024];
	swprintf_s(buffer, _countof(buffer), L"%S", name, args...);
	obj->SetName(buffer);
}

#endif