#pragma once

#include "Axiom/Core/Common.hpp"

#include <iostream>

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
		char lineNum[32];\
		sprintf_s(lineNum, "%u", __LINE__);\
		std::cout << ("Error in: ");\
		std::cout << (__FILE__);\
		std::cout << ("\nLine: ");\
		std::cout << (lineNum);\
		std::cout << ("\n");\
		std::cout << (#x);\
		std::cout << ("\n");\
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