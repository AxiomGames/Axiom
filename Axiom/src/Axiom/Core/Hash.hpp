/*-
 * this file is based on: https://github.com/mendsley/tinystl
 *
 * Copyright 2012-2018 Matthew Endsley
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providing that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "Common.hpp"

constexpr static Hash_t HashRaw(const char* data, Hash_t size)
{
	Hash_t hash = 0;

	typedef const char* pointer;
	for (pointer it = data, end = data + size; it != end; ++it)
	{
		hash = *it + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}

template<typename T>
struct Hash
{
	constexpr static bool c_HasHashImpl = true;

	constexpr static Hash_t GenerateHash(const T& value)
	{
		return HashRaw((const char*)&value, sizeof(T));
	}
};

template<typename TValue>
inline constexpr Hash_t HashValue(const TValue& value)
{
	static_assert(Hash<TValue>::c_HasHashImpl, "type has no hash implementation");
	return Hash<TValue>::GenerateHash(value);
}

