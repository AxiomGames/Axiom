#include "Memory.hpp"

IAllocator* GetDefaultAllocator()
{
	static UniquePtr<DefaultAllocator> allocator = std::make_unique<DefaultAllocator>();
	return allocator.get();
}

MemPtr Memory::Malloc(uint64 size, uint64_t alignment)
{
	IAllocator* allocator = GetDefaultAllocator();
	return allocator->Malloc(size, alignment);
}

MemPtr Memory::ReAlloc(MemPtr original, uint64 size, uint64_t alignment)
{
	return GetDefaultAllocator()->ReAlloc(original, size, alignment);
}

void Memory::Free(VoidPtr ptr)
{
	GetDefaultAllocator()->Free(ptr);
}
