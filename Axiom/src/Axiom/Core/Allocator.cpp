#include "Allocator.hpp"
#include <memory>

MemPtr DefaultAllocator::Malloc(uint64 size, uint64_t alignment)
{
	return reinterpret_cast<MemPtr>(std::malloc(size));
}

MemPtr DefaultAllocator::ReAlloc(MemPtr ptr, uint64 size, uint64_t alignment)
{
	return reinterpret_cast<MemPtr>(std::realloc(ptr, Align(size, alignment)));
}

void DefaultAllocator::Free(VoidPtr pointer)
{
	std::free(pointer);
}
