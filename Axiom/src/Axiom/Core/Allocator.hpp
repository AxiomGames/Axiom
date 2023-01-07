#pragma once

#include "Common.hpp"
#include "TemplateBase.hpp"
#include <malloc.h>

using MemPtr = uint8*;
using VoidPtr = void*;

template<typename T>
constexpr T Align(T val, uint64_t alignment)
{
	if (alignment == 0)
	{
		return val;
	}

	uint64_t mask = ~(alignment - 1);
	uint64_t final = ((uint64_t) val + alignment - 1) & mask;
	return (T) final;
}

template<typename T>
constexpr T IsAligned(T val, uint64_t alignment)
{
	return !((uint64_t) val & (alignment - 1));
}

FINLINE void MemCopy(MemPtr dest, MemPtr src, uint64 size)
{
	std::memcpy(dest, src, size);
}

FINLINE void MemZero(MemPtr src, uint64 size)
{
	std::memset(src, 0, size);
}

class IAllocator
{
public:
	virtual ~IAllocator() = default;

	virtual MemPtr Malloc(uint64 size, uint64_t alignment = 0) = 0;
	virtual MemPtr ReAlloc(MemPtr ptr, uint64 size, uint64_t alignment = 0) = 0;
	virtual void Free(VoidPtr pointer) = 0;

	template<typename T, typename... Args>
	FINLINE T* Alloc(Args... args)
	{
		MemPtr ptr = Malloc(sizeof(T));
		return new(ptr) T(Forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	FINLINE T* AllocAligned(uint64_t alignment, Args... args)
	{
		MemPtr ptr = Malloc(sizeof(T), alignment);
		return new(ptr) T(Forward<Args>(args)...);
	}

	template<typename T>
	FINLINE T* ReAlloc(T* ptr, uint64 size)
	{
		return reinterpret_cast<T*>(ReAlloc(reinterpret_cast<MemPtr>(ptr), size));
	}

	template<typename T>
	FINLINE void FreeDestruct(VoidPtr ptr)
	{
		T* type = reinterpret_cast<T*>(ptr);
		type->~T();
		Free(ptr);
	}
};

class DefaultAllocator : public IAllocator
{
public:
	MemPtr Malloc(uint64 size, uint64_t alignment) override;
	MemPtr ReAlloc(MemPtr ptr, uint64 size, uint64_t alignment) override;
	void Free(VoidPtr pointer) override;
};
