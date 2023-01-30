#pragma once

#include "Axiom/Core/Common.hpp"

enum class EShaderType : uint32
{
	None,
	Vertex,
	Fragment,
	Compute
};

struct IShader
{
	const char* sourceCode = nullptr;
	struct ByteCode
	{
		void* blob = nullptr;
		size_t blobSize = 0;
	} byteCode{};
};