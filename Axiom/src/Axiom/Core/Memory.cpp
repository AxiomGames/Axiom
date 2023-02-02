#include "Memory.hpp"

MemPtr Memory::Malloc(uint64 size, uint64_t alignment)
{
	return GDefaultAllocator.Malloc(size, alignment);
}

MemPtr Memory::ReAlloc(MemPtr original, uint64 size, uint64_t alignment)
{
	return GDefaultAllocator.ReAlloc(original, size, alignment);
}

void Memory::Free(VoidPtr ptr)
{
	GDefaultAllocator.Free(ptr);
}
