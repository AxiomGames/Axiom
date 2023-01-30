#pragma once

#include "Types.hpp"

struct BufferDesc
{
	EResourceUsage ResourceUsage = EResourceUsage::Unknown;
	uint64 Size = 0;
	uint64 ElementByteStride = 0;
	void* Data;
};

struct IBuffer
{
	EResourceUsage ResourceUsage;
	uint64 SizeInBytes = 0;
	uint64 ElementByteStride = 0;
	uint64 DataLen = 0;
	uint64 DataOffset = 0;

	void* BufferData;
};
