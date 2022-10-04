#pragma once

#include "Array.hpp"
#include "robin_hood.h"

typedef uint8_t* MemPtr;
typedef uint32_t MemSize;
static constexpr MemSize MemSizeOf = sizeof(MemSize);

template<typename T>
constexpr T Align(T val, uint64_t alignment)
{
	uint64_t mask = ~(alignment - 1);
	uint64_t final = ((uint64_t) val + alignment - 1) & mask;
	return (T) final;
}

template<typename T>
constexpr T IsAligned(T val, uint64_t alignment)
{
	return !((uint64_t) val & (alignment - 1));
}

struct CMBMemoryLocation
{
	uint32_t BlockIndex;
	MemSize Offset;
	MemSize Size;
};

class CMBAllocator
{
public:
	struct MemoryFragment
	{
		MemPtr Begin;
		MemPtr End;
		MemSize Size;
	};

	struct MemoryBlock
	{
		MemPtr Memory;
		Array<MemoryFragment> Fragments; // TODO: Change to binary tree for memory size optimizations
		MemSize FreeMemory;
	};
private:
	MemSize m_BlockSize;
	Array<MemoryBlock> m_Memory;
	std::unordered_map<uintptr_t, MemSize> m_MemorySizes;
public:

	explicit CMBAllocator(MemSize blockSize = 8388608);
	~CMBAllocator();

	MemPtr AllocRaw(MemSize size);
	void FreeRaw(void* ptr);
	bool CheckMemory(void* ptr) const;
	CMBMemoryLocation GetMemoryLocation(void* ptr) const;

	template<class T, typename... Args>
	FINLINE T* Alloc(Args&&... args)
	{
		T* type = reinterpret_cast<T*>(AllocRaw(sizeof(T)));
		new (type) T(std::forward<Args>(args)...);
		return type;
	}

	template<class T, typename... Args>
	FINLINE T* AllocAligned(uint64_t alignment, Args&&... args)
	{
		T* type = reinterpret_cast<T*>(AllocRaw(Align(sizeof(T), alignment)));
		new (type) T(std::forward<Args>(args)...);
		return type;
	}

	template<class T>
	FINLINE void Free(void* ptr)
	{
		T* type = reinterpret_cast<T*>(ptr);
		type->~T();
		FreeRaw(ptr);
	}
private:
	MemoryBlock& AllocateMemoryBlock();
	MemPtr AllocFromFragment(MemoryBlock& memoryBlock, const Array<MemoryFragment>::iterator& framentIt, MemSize size);
};
