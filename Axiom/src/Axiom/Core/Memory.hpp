#pragma once

#include "Allocator.hpp"

struct AX_API Memory
{
	static MemPtr Malloc(uint64 size, uint64_t alignment = 0);
	static MemPtr ReAlloc(MemPtr original, uint64 size, uint64_t alignment = 0);
	static void Free(VoidPtr ptr);

	static FINLINE MemPtr MallocZeroed(uint64 size, uint64_t alignment = 0)
	{
		MemPtr ptr = Malloc(size, alignment);
		MemZero(ptr, size);
		return ptr;
	}

	template<typename T, typename... Args>
	static FINLINE T* Alloc(Args... args)
	{
		MemPtr ptr = Malloc(sizeof(T));
		return new(ptr) T(Forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	static FINLINE T* AllocAligned(uint64_t alignment, Args... args)
	{
		MemPtr ptr = Malloc(sizeof(T), alignment);
		return new(ptr) T(Forward<Args>(args)...);
	}

	template<typename T>
	static FINLINE T* ReAlloc(T* ptr, uint64 size, uint64_t alignment = 0)
	{
		return reinterpret_cast<T*>(ReAlloc(reinterpret_cast<MemPtr>(ptr), size, alignment));
	}

	template<typename T>
	static FINLINE void FreeDestruct(VoidPtr ptr)
	{
		T* type = reinterpret_cast<T*>(ptr);
		type->~T();
		Free(ptr);
	}
};
