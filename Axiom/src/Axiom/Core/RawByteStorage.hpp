#pragma once

#include "Common.hpp"

template<int32 SIze, uint32 Alignment>
struct TAlignedBytes;

template<int32 Size>
struct TAlignedBytes<Size, 1>
{
	uint8 Data[Size];
};

#define IMPLEMENT_ALIGNED(Align) template<int32 Size> struct TAlignedBytes<Size, Align> { struct alignas(Align) PaddingContainer { uint8 Data[Size]; }; PaddingContainer Padding; };

IMPLEMENT_ALIGNED(64)
IMPLEMENT_ALIGNED(32)
IMPLEMENT_ALIGNED(16)
IMPLEMENT_ALIGNED(8)
IMPLEMENT_ALIGNED(4)
IMPLEMENT_ALIGNED(2)

#undef IMPLEMENT_ALIGNED

template<typename T>
struct RawByteStorage : public TAlignedBytes<sizeof(T), alignof(T)>
{
	T* GetPtr() { return (T*)this; }
	const T* GetPtr() const { return (const T*)this; }
};
