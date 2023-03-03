/*-
 * Copyright 2012-1017 Matthew Endsley
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
#include "String.hpp"

template<typename Type>
class BasicStringView
{
public:
	typedef Type ValueType;
	typedef Type* Pointer;
	typedef const Type* ConstPointer;
	typedef ConstPointer Iterator;
	typedef ConstPointer ConstIterator;

	static constexpr std::size_t npos = std::size_t(-1);

	constexpr BasicStringView();

	constexpr BasicStringView(const Type* s, std::size_t count);

	constexpr BasicStringView(const Type* s);

	template<typename Alloc>
	constexpr BasicStringView(const BasicString<Type, Alloc>& string);

	constexpr BasicStringView(const BasicStringView&) = default;

	BasicStringView& operator=(const BasicStringView&) = default;

	constexpr const Type* Data() const;

	constexpr ConstPointer CStr() const;

	constexpr char operator[](std::size_t pos) const;

	constexpr std::size_t Size() const;

	constexpr bool Empty() const;

	constexpr Iterator begin() const;

	constexpr ConstIterator cbegin() const;

	constexpr Iterator end() const;

	constexpr ConstIterator cend() const;

	constexpr BasicStringView Substr(std::size_t pos = 0, std::size_t count = npos) const;

	constexpr void Swap(BasicStringView& v);

	constexpr int32 Compare(const BasicStringView& other) const;

	constexpr int32 Compare(ConstPointer sz) const;

private:
	BasicStringView(decltype(nullptr)) = delete;

	static constexpr std::size_t Strlen(const Type*);

	const Type* m_Str;
	std::size_t m_Size;
};

template<typename Type>
template<typename Alloc>
constexpr BasicStringView<Type>::BasicStringView(const BasicString<Type, Alloc>& string) : m_Str(string.CStr()), m_Size(string.Size())
{
}

template<typename Type>
constexpr BasicStringView<Type>::BasicStringView() : m_Str(nullptr), m_Size(0)
{
}

template<typename Type>
constexpr BasicStringView<Type>::BasicStringView(const Type* s, std::size_t count) : m_Str(s), m_Size(count)
{
}

template<typename Type>
constexpr BasicStringView<Type>::BasicStringView(const Type* s) : m_Str(s), m_Size(Strlen(s))
{
}

template<typename Type>
constexpr const Type* BasicStringView<Type>::Data() const
{
	return m_Str;
}

template<typename Type>
constexpr typename BasicStringView<Type>::ConstPointer BasicStringView<Type>::CStr() const
{
	return m_Str;
}

template<typename Type>
constexpr char BasicStringView<Type>::operator[](std::size_t pos) const
{
	return m_Str[pos];
}

template<typename Type>
constexpr std::size_t BasicStringView<Type>::Size() const
{
	return m_Size;
}

template<typename Type>
constexpr bool BasicStringView<Type>::Empty() const
{
	return 0 == m_Size;
}

template<typename Type>
constexpr typename BasicStringView<Type>::Iterator BasicStringView<Type>::begin() const
{
	return m_Str;
}

template<typename Type>
constexpr typename BasicStringView<Type>::ConstIterator BasicStringView<Type>::cbegin() const
{
	return m_Str;
}

template<typename Type>
constexpr typename BasicStringView<Type>::Iterator BasicStringView<Type>::end() const
{
	return m_Str + m_Size;
}

template<typename Type>
constexpr typename BasicStringView<Type>::ConstIterator BasicStringView<Type>::cend() const
{
	return m_Str + m_Size;
}

template<typename Type>
constexpr BasicStringView<Type> BasicStringView<Type>::Substr(std::size_t pos, std::size_t count) const
{
	return BasicStringView(m_Str + pos, npos == count ? m_Size - pos : count);
}

template<typename Type>
constexpr void BasicStringView<Type>::Swap(BasicStringView<Type>& v)
{
	Type* strtmp = m_Str;
	std::size_t sizetmp = m_Size;
	m_Str = v.m_Str;
	m_Size = v.m_Size;
	v.m_Str = strtmp;
	v.m_Size = sizetmp;
}

template<typename Type>
constexpr std::size_t BasicStringView<Type>::Strlen(const Type* s)
{
	for (std::size_t len = 0;; ++len)
	{
		if (0 == s[len])
		{
			return len;
		}
	}
}

template<typename Type>
constexpr int32 BasicStringView<Type>::Compare(const BasicStringView<Type>& other) const
{
	return Compare(other.CStr());
}

template<typename Type>
constexpr int32 BasicStringView<Type>::Compare(typename BasicStringView<Type>::ConstPointer sz) const
{
	ConstPointer it = CStr();
	for (; *it && *sz && (*it == *sz); ++it, ++sz);
	return *it - *sz;
}

template<typename Type>
FINLINE bool operator==(const BasicStringView<Type>& lhs, const BasicStringView<Type>& rhs)
{
	if (lhs.Size() != rhs.Size())
	{
		return false;
	}

	return lhs.Compare(rhs) == 0;
}

template<typename Type>
FINLINE bool operator==(const BasicStringView<Type>& lhs, typename BasicStringView<Type>::ConstPointer rhs)
{
	return lhs.Compare(rhs) == 0;
}

template<typename Type>
FINLINE bool operator==(typename BasicStringView<Type>::ConstPointer lhs, const BasicStringView<Type>& rhs)
{
	return rhs.Compare(lhs) == 0;
}

template<typename Type>
FINLINE bool operator!=(const BasicStringView<Type>& lhs, const BasicStringView<Type>& rhs)
{
	return !(lhs == rhs);
}

template<typename Type>
FINLINE bool operator!=(const BasicStringView<Type>& lhs, typename BasicStringView<Type>::ConstPointer rhs)
{
	return !(lhs == rhs);
}

template<typename Type>
FINLINE bool operator!=(typename BasicStringView<Type>::ConstPointer lhs, const BasicStringView<Type>& rhs)
{
	return !(lhs == rhs);
}

template<typename Type>
FINLINE bool operator<(const BasicStringView<Type>& lhs, const BasicStringView<Type>& rhs)
{
	return lhs.Compare(rhs) < 0;
}

template<typename Type>
FINLINE bool operator<(const BasicStringView<Type>& lhs, typename BasicStringView<Type>::ConstPointer rhs)
{
	return lhs.Compare(rhs) < 0;
}

template<typename Type>
FINLINE bool operator<(typename BasicStringView<Type>::ConstPointer lhs, const BasicStringView<Type>& rhs)
{
	return rhs.Compare(lhs) > 0;
}

template<typename Type>
FINLINE bool operator>(const BasicStringView<Type>& lhs, const BasicStringView<Type>& rhs)
{
	return rhs < lhs;
}

template<typename Type>
FINLINE bool operator>(const BasicStringView<Type>& lhs, typename BasicStringView<Type>::ConstPointer rhs)
{
	return rhs < lhs;
}

template<typename Type>
FINLINE bool operator>(typename BasicStringView<Type>::ConstPointer lhs, const BasicStringView<Type>& rhs)
{
	return rhs < lhs;
}

template<typename Type>
FINLINE bool operator<=(const BasicStringView<Type>& lhs, const BasicStringView<Type>& rhs)
{
	return !(rhs < lhs);
}

template<typename Type>
FINLINE bool operator<=(const BasicStringView<Type>& lhs, typename BasicStringView<Type>::ConstPointer rhs)
{
	return !(rhs < lhs);
}

template<typename Type>
FINLINE bool operator<=(typename BasicStringView<Type>::ConstPointer lhs, const BasicStringView<Type>& rhs)
{
	return !(rhs < lhs);
}

template<typename Type>
FINLINE bool operator>=(const BasicStringView<Type>& lhs, const BasicStringView<Type>& rhs)
{
	return !(lhs < rhs);
}

template<typename Type>
FINLINE bool operator>=(const BasicStringView<Type>& lhs, typename BasicStringView<Type>::ConstPointer rhs)
{
	return !(lhs < rhs);
}

template<typename Type>
FINLINE bool operator>=(typename BasicStringView<Type>::ConstPointer lhs, const BasicStringView<Type>& rhs)
{
	return !(lhs < rhs);
}

template<typename Element>
struct Hash<BasicStringView<Element>>
{
	constexpr static bool c_HasHashImpl = true;

	constexpr static Hash_t GenerateHash(const BasicStringView<Element>& string)
	{
		Hash_t hash = 0;
		for (auto it = string.begin(); it != string.end(); ++it)
		{
			hash = *it + (hash << 6) + (hash << 16) - hash;
		}
		return hash;
	}
};


using StringView = BasicStringView<char>;
using WStringView = BasicStringView<wchar_t>;