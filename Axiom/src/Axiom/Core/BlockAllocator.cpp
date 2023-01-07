#include "BlockAllocator.hpp"

BlockAllocator::BlockAllocator(uint64 blockSize) : m_BlockSize(blockSize)
{
	AllocateMemoryBlock();
}

BlockAllocator::~BlockAllocator()
{
	for (const auto &item : m_Memory)
	{
		delete[] item.Memory;
	}
}

MemPtr BlockAllocator::Malloc(uint64 size, uint64_t alignment)
{
	size = Align(size, alignment);

	ax_assert(size);
	ax_assert(size <= m_BlockSize);

	for (MemoryBlock& memoryBlock : m_Memory)
	{
		if (memoryBlock.FreeMemory < size)
		{
			continue;
		}

		// Check for memory fragments
		for (auto fragmentIt = memoryBlock.Fragments.begin(); fragmentIt != memoryBlock.Fragments.end(); fragmentIt++)
		{
			if (fragmentIt->Size >= size)
			{
				return AllocFromFragment(memoryBlock, fragmentIt, size);
			}
		}
	}

	MemoryBlock& newBlock = AllocateMemoryBlock();
	return AllocFromFragment(newBlock, newBlock.Fragments.begin(), size);
}

MemPtr BlockAllocator::ReAlloc(MemPtr ptr, uint64 size, uint64_t alignment)
{
	if (ptr == nullptr)
	{
		return nullptr;
	}

	MemPtr newLocation = Malloc(size, alignment);
	MemCopy(newLocation, ptr, size);
	Free(ptr);
	return newLocation;
}

void BlockAllocator::Free(VoidPtr ptr)
{
	if (ptr == nullptr)
	{
		return;
	}

	MemPtr memPtrBegin = reinterpret_cast<MemPtr>(ptr);
	auto it = m_MemorySizes.find((uintptr_t)memPtrBegin);

	if (it == m_MemorySizes.end())
	{
		ax_assert(0);
		//AU_LOG_FATAL("Cound not find size for pointer ", PointerToString(ptr), " !");
		return;
	}

	uint64 size = it->second;
	MemPtr memPtrEnd = memPtrBegin + size;

	m_MemorySizes.erase(it);

	// TODO: Think if free should clear the memory to 0
	std::memset(memPtrBegin, 0, size);

	for (MemoryBlock& memoryBlock : m_Memory)
	{
		if (memPtrBegin >= memoryBlock.Memory && memPtrEnd <= (memoryBlock.Memory + m_BlockSize))
		{
			// Insert free fragment at the beginning
			memoryBlock.Fragments.EmplaceAt(0, MemoryFragment{memPtrBegin, memPtrEnd, size});
			memoryBlock.FreeMemory += size;

			// TODO: merge blocks with same begin or end

			return;
		}
	}

	//AU_LOG_FATAL("Memory ", PointerToString(ptr), " is not part of this allocator !");
}

bool BlockAllocator::CheckMemory(void* ptr) const
{
	if (ptr == nullptr)
	{
		return false;
	}

	MemPtr memPtrBegin = reinterpret_cast<MemPtr>(ptr);

	auto it = m_MemorySizes.find((uintptr_t)memPtrBegin);

	if (it == m_MemorySizes.end())
	{
		return false;
	}

	return true;
}

BlockAllocator::MemoryBlock& BlockAllocator::AllocateMemoryBlock()
{
	MemoryBlock memoryBlock;
	memoryBlock.Memory = new uint8_t[m_BlockSize];
	memoryBlock.Fragments.Add(MemoryFragment{memoryBlock.Memory, memoryBlock.Memory + m_BlockSize, m_BlockSize});
	memoryBlock.FreeMemory = m_BlockSize;

	memset(memoryBlock.Memory, 0, m_BlockSize);

	return m_Memory.Add(memoryBlock);
}

FINLINE MemPtr BlockAllocator::AllocFromFragment(MemoryBlock& memoryBlock, const Array<MemoryFragment>::Iterator& fragmentIt, uint64 size)
{
	MemoryFragment& fragment = *fragmentIt;

	MemPtr begin = fragment.Begin;
	// Get new memory start
	MemPtr newMemoryStart = begin;

	// Decrement block free size
	memoryBlock.FreeMemory -= size;

	// Change fragment size and if remaining is 0 then delete fragment
	fragment.Begin = newMemoryStart + size;
	fragment.Size -= size;
	ax_assert(fragment.Size >= 0);
	if (fragment.Size == 0)
	{
		memoryBlock.Fragments.Remove(fragmentIt);
	}

	m_MemorySizes[(uintptr_t)newMemoryStart] = size;

	// FIXME: when new block is allocated the memory is somehow invalid

	// Return new memory
	return newMemoryStart;
}
