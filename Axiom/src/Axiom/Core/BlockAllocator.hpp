#pragma once

#include "Array.hpp"
#include "robin_hood.h"
#include "Allocator.hpp"

class BlockAllocator : public IAllocator
{
public:
	struct MemoryFragment
	{
		MemPtr Begin;
		MemPtr End;
		uint64 Size;
	};

	struct MemoryBlock
	{
		MemPtr Memory;
		Array<MemoryFragment> Fragments; // TODO: Change to binary tree for memory size optimizations
		uint64 FreeMemory;
	};
private:
	uint64 m_BlockSize;
	Array<MemoryBlock> m_Memory;
	std::unordered_map<uintptr_t, uint64> m_MemorySizes;
public:

	explicit BlockAllocator(uint64 blockSize = 8'388'608);
	~BlockAllocator() override;

	MemPtr Malloc(uint64 size, uint64_t alignment = 0) override;
	MemPtr ReAlloc(MemPtr ptr, uint64 size, uint64_t alignment = 0) override;

	void Free(VoidPtr ptr) override;
	bool CheckMemory(void* ptr) const;

	[[nodiscard]] FINLINE uint32 GetNumberOfBlocks() const
	{ return m_Memory.Count(); }
private:
	MemoryBlock& AllocateMemoryBlock();
	MemPtr AllocFromFragment(MemoryBlock& memoryBlock, const Array<MemoryFragment>::Iterator& fragmentIt, uint64 size);
};
