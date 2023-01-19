/*-
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


#include <string_view>
#include "Memory.hpp"
#include "Hash.hpp"

#pragma once

typedef uint64_t StrHashID;

template<typename T>
constexpr StrHashID HashDjb2(const T* str)
{
	uint64 hash = 5381;

	T c = *str;
	while (c != 0)
	{
		hash = ((hash << 5) + hash) + c;
		c = *(++str);
	}

	return hash;
}

constexpr StrHashID HashDjb2(const std::string_view& view)
{
	uint64 hash = 5381;

	for (const char c: view)
	{
		hash = ((hash << 5) + hash) + c;
	}

	return hash;
}

StrHashID constexpr operator "" _HASH(const char* s, size_t)
{
	return HashDjb2(s);
}

template<typename Type>
struct StringConverter
{
	constexpr static bool c_HasConverter = false;
};

template<typename T, typename Alloc = DEFAULT_ALLOCATOR>
class BasicString
{
public:
	using Type = T;
	using Pointer = T*;
	using ConstPointer = const T*;

	using Iterator = T*;
	using ConstIterator = const T*;

	BasicString();

	BasicString(const BasicString& other);

	BasicString(BasicString&& other) noexcept;

	BasicString(ConstPointer sz);

	BasicString(ConstPointer first, ConstPointer last);

	BasicString(ConstPointer sz, size_t len);

	~BasicString();

	BasicString& operator=(const BasicString& other);

	BasicString& operator=(BasicString&& other) noexcept ;

	BasicString& operator=(Type ch);

	BasicString& operator=(ConstPointer sz);

	BasicString& operator+=(const BasicString& other);

	BasicString& operator+=(Type ch);

	BasicString& operator+=(ConstPointer sz);

	ConstPointer CStr() const;

	bool Empty() const;

	size_t Size() const;

	size_t Capacity() const;

	Iterator begin();

	Iterator end();

	ConstIterator begin() const;

	ConstIterator end() const;

	Type operator[](size_t pos) const;

	Type& operator[](size_t pos);

	int Compare(const BasicString& other) const;

	int Compare(ConstPointer sz) const;

	void Reserve(size_t Capacity);

	void Resize(size_t n);

	void Resize(size_t n, Type ch);

	void Clear();

	void Assign(Type ch);

	void Assign(ConstPointer sz);

	void Assign(ConstPointer first, ConstPointer last);

	void Assign(const BasicString& other);

	void PushBack(Type ch);

	void Append(ConstPointer sz);

	template<typename Type>
	void Append(const Type& appendType)
	{
		static_assert(StringConverter<Type>::c_HasConverter);
		StringConverter<Type>::Append(*this, appendType);
	}

	void Append(ConstPointer first, ConstPointer last);

	void Append(const BasicString& other);

	void Insert(Iterator where, Type ch);

	void Insert(Iterator where, ConstPointer sz);

	void Insert(Iterator where, ConstPointer first, ConstPointer last);

	void Insert(Iterator where, const BasicString& other);

	void Erase(Iterator first, Iterator last);

	void Swap(BasicString& other);

private:
	static const size_t c_NBuffer = 16;
	static const size_t c_LongFlag = ((size_t) 1) << (sizeof(size_t) * 8 - 1);
	size_t m_Size;
	union
	{
		struct
		{
			Pointer m_First;
			Pointer m_Capacity;
		};
		Type m_Buffer[c_NBuffer];
	};
};

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>::BasicString()
	: m_Size(0)
{
	m_Buffer[0] = 0;
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>::BasicString(const BasicString& other)
	: m_Size(0)
{
	Assign(other);
}

template<typename T, typename Alloc>
BasicString<T, Alloc>::BasicString(BasicString&& other)  noexcept : m_Size(0)
{
	if (m_Size & c_LongFlag)
	{
		Reserve(other.Size());
		Append(other.begin(), other.end());
	}
	else
	{
		m_Capacity = other.m_Capacity;
		m_First = other.m_First;
		m_Size = other.m_Size;
	}
	other.m_Size = 0;
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>::BasicString(ConstPointer sz)
	: m_Size(0)
{
	Assign(sz);
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>::BasicString(ConstPointer first, ConstPointer last)
	: m_Size(0)
{
	Assign(first, last);
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>::BasicString(ConstPointer sz, size_t len)
	: m_Size(0)
{
	Append(sz, sz + len);
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>::~BasicString()
{
	if (m_Size & c_LongFlag)
	{
		Alloc::Free(m_First);
	}
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>& BasicString<T, Alloc>::operator=(const BasicString& other)
{
	if (this != &other)
	{
		Assign(other);
	}
	return *this;
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>& BasicString<T, Alloc>::operator=(BasicString&& other) noexcept
{
	if (m_Size & c_LongFlag)
	{
		Reserve(other.Size());
		Append(other.begin(), other.end());
	}
	else
	{
		m_Capacity = other.m_Capacity;
		m_First = other.m_First;
		m_Size = other.m_Size;
	}
	other.m_Size = 0;

	return *this;
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>& BasicString<T, Alloc>::operator=(Type ch)
{
	Assign(ch);
	return *this;
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>& BasicString<T, Alloc>::operator=(ConstPointer sz)
{
	Assign(sz);
	return *this;
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>& BasicString<T, Alloc>::operator+=(const BasicString& other)
{
	Append(other);
	return *this;
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>& BasicString<T, Alloc>::operator+=(Type ch)
{
	PushBack(ch);
	return *this;
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>& BasicString<T, Alloc>::operator+=(ConstPointer sz)
{
	Append(sz);
	return *this;
}

template<typename T, typename Alloc>
FINLINE typename BasicString<T, Alloc>::ConstPointer BasicString<T, Alloc>::CStr() const
{
	if (m_Size & c_LongFlag)
	{
		return m_First;
	}
	else
	{
		return m_Buffer;
	}

}

template<typename T, typename Alloc>
FINLINE bool BasicString<T, Alloc>::Empty() const
{
	return Size() == 0;
}

template<typename T, typename Alloc>
FINLINE size_t BasicString<T, Alloc>::Size() const
{
	return m_Size & ~c_LongFlag;
}

template<typename T, typename Alloc>
FINLINE size_t BasicString<T, Alloc>::Capacity() const
{
	if (m_Size & c_LongFlag)
	{
		return m_Capacity - m_First - 1;
	}
	else
	{
		return c_NBuffer - 1;
	}
}

template<typename T, typename Alloc>
FINLINE typename BasicString<T, Alloc>::Iterator BasicString<T, Alloc>::begin()
{
	if (m_Size & c_LongFlag)
	{
		return m_First;
	}
	else
	{
		return m_Buffer;
	}
}

template<typename T, typename Alloc>
FINLINE typename BasicString<T, Alloc>::Iterator BasicString<T, Alloc>::end()
{
	if (m_Size & c_LongFlag)
	{
		return m_First + (m_Size & ~c_LongFlag);
	}
	else
	{
		return m_Buffer + m_Size;
	}
}

template<typename T, typename Alloc>
FINLINE typename BasicString<T, Alloc>::ConstIterator BasicString<T, Alloc>::begin() const
{
	if (m_Size & c_LongFlag)
	{
		return m_First;
	}
	else
	{
		return m_Buffer;
	}
}

template<typename T, typename Alloc>
FINLINE typename BasicString<T, Alloc>::ConstIterator BasicString<T, Alloc>::end() const
{
	if (m_Size & c_LongFlag)
	{
		return m_First + (m_Size & ~c_LongFlag);
	}
	else
	{
		return m_Buffer + m_Size;
	}
}

template<typename T, typename Alloc>
FINLINE typename BasicString<T, Alloc>::Type BasicString<T, Alloc>::operator[](size_t pos) const
{
	return begin()[pos];
}

template<typename T, typename Alloc>
FINLINE typename BasicString<T, Alloc>::Type& BasicString<T, Alloc>::operator[](size_t pos)
{
	return begin()[pos];
}

template<typename T, typename Alloc>
FINLINE int BasicString<T, Alloc>::Compare(const BasicString& other) const
{
	return Compare(other.CStr());
}

template<typename T, typename Alloc>
FINLINE int BasicString<T, Alloc>::Compare(ConstPointer sz) const
{
	ConstPointer it = CStr();
	for (; *it && *sz && (*it == *sz); ++it, ++sz);
	return *it - *sz;
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::Reserve(size_t cap)
{
	if (cap <= Capacity())
		return;

	auto newfirst = (Pointer) Alloc::Malloc(cap + 1);
	for (Pointer it = begin(), newit = newfirst, e = end() + 1; it != e; ++it, ++newit)
	{
		*newit = *it;
	}

	if (m_Size & c_LongFlag)
	{
		Alloc::Free(m_First);
	}
	else
	{
		m_Size |= c_LongFlag;
	}

	m_First = newfirst;
	m_Capacity = m_First + cap + 1;
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::Resize(size_t n)
{
	Resize(n, 0);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::Resize(size_t n, Type ch)
{
	if (Size() < n)
	{
		Reserve(n);
		for (Pointer it = end(), e = begin() + n; it != e; ++it)
		{
			*it = ch;
		}
	}
	Pointer it = begin() + n;
	*it = 0;
	m_Size = n | (m_Size & c_LongFlag);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::Clear()
{
	Resize(0);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::Assign(Type ch)
{
	Pointer it = begin();
	*it = ch;
	*(it + 1) = 0;
	m_Size = 1 | (m_Size & c_LongFlag);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::Assign(ConstPointer sz)
{
	size_t len = 0;
	for (ConstPointer it = sz; *it; ++it)
	{
		++len;
	}

	Assign(sz, sz + len);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::Assign(ConstPointer first, ConstPointer last)
{
	size_t newsize = last - first;
	Reserve(newsize);

	Pointer newit = begin();
	for (ConstPointer it = first; it != last; ++it, ++newit)
	{
		*newit = *it;
	}
	*newit = 0;
	m_Size = newsize | (m_Size & c_LongFlag);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::Assign(const BasicString& other)
{
	Assign(other.begin(), other.end());
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::PushBack(Type ch)
{
	if (Size() != Capacity())
	{
		Pointer it = end();
		*it = ch;
		*(it + 1) = 0;
		++m_Size;
	}
	else
	{
		Append(&ch, &ch + 1);
	}
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::Append(ConstPointer sz)
{
	size_t len = 0;
	for (ConstPointer it = sz; *it; ++it)
	{
		++len;
	}
	Append(sz, sz + len);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::Append(ConstPointer first, ConstPointer last)
{
	const size_t newsize = (size_t) (Size() + (last - first));
	if (newsize > Capacity())
	{
		Reserve((newsize * 3) / 2);
	}

	Pointer newit = end();
	for (ConstPointer it = first; it != last; ++it, ++newit)
	{
		*newit = *it;
	}

	*newit = 0;
	m_Size = newsize | (m_Size & c_LongFlag);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::Append(const BasicString& other)
{
	Append(other.begin(), other.end());
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::Insert(Iterator where, Type ch)
{
	Insert(where, &ch, &ch + 1);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::Insert(Iterator where, ConstPointer sz)
{
	size_t len = 0;
	for (ConstPointer it = sz; *it; ++it)
	{
		++len;
	}
	Insert(where, sz, sz + len);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::Insert(Iterator where, ConstPointer first, ConstPointer last)
{
	if (first == last)
	{
		return;
	}

	const size_t w = where - begin();
	const size_t newsize = (size_t) (Size() + (last - first));

	if (newsize > Capacity())
	{
		Reserve((newsize * 3) / 2);
	}

	Pointer newit = begin() + newsize;
	for (Pointer it = end(), e = begin() + w; it >= e; --it, --newit)
	{
		*newit = *it;
	}

	newit = begin() + w;
	for (ConstPointer it = first; it != last; ++it, ++newit)
	{
		*newit = *it;
	}

	m_Size = newsize | (m_Size & c_LongFlag);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::Insert(Iterator where, const BasicString& other)
{
	Insert(where, other.begin(), other.end());
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::Erase(Iterator first, Iterator last)
{
	if (first == last)
	{
		return;
	}

	const size_t newsize = (size_t) (Size() + (first - last));
	Pointer newit = first;
	for (Pointer it = last, e = end(); it != e; ++it, ++newit)
	{
		*newit = *it;
	}

	*newit = 0;
	m_Size = newsize | (m_Size & c_LongFlag);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::Swap(BasicString& other)
{
	const size_t tsize = m_Size;
	Pointer tfirst, tcapacity;
	Type tbuffer[c_NBuffer];

	if (tsize & c_LongFlag)
	{
		tfirst = m_First;
		tcapacity = m_Capacity;
	}
	else
	{
		for (Pointer it = m_Buffer, newit = tbuffer, e = m_Buffer + tsize + 1; it != e; ++it, ++newit)
			*newit = *it;
	}

	m_Size = other.m_Size;
	if (other.m_Size & c_LongFlag)
	{
		m_First = other.m_First;
		m_Capacity = other.m_Capacity;
	}
	else
	{
		for (Pointer it = other.m_Buffer, newit = m_Buffer, e = other.m_Buffer + m_Size + 1; it != e; ++it, ++newit)
			*newit = *it;
	}

	other.m_Size = tsize;
	if (tsize & c_LongFlag)
	{
		other.m_First = tfirst;
		other.m_Capacity = tcapacity;
	}
	else
	{
		for (Pointer it = tbuffer, newit = other.m_Buffer, e = tbuffer + tsize + 1; it != e; ++it, ++newit)
			*newit = *it;
	}
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc> operator+(const BasicString<T, Alloc>& lhs, const BasicString<T, Alloc>& rhs)
{
	BasicString<T, Alloc> ret;
	ret.Reserve(lhs.Size() + rhs.Size());
	ret += lhs;
	ret += rhs;
	return ret;
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc> operator+(const BasicString<T, Alloc>& lhs, typename BasicString<T, Alloc>::ConstPointer rhs)
{
	BasicString ret = lhs;
	ret += rhs;
	return ret;
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc> operator+(typename BasicString<T, Alloc>::ConstPointer lhs, const BasicString<T, Alloc>& rhs)
{
	BasicString<T, Alloc> ret = lhs;
	ret += rhs;
	return ret;
}

template<typename T, typename Alloc>
FINLINE bool operator==(const BasicString<T, Alloc>& lhs, const BasicString<T, Alloc>& rhs)
{
	if (lhs.Size() != rhs.Size())
	{
		return false;
	}

	return lhs.Compare(rhs) == 0;
}

template<typename T, typename Alloc>
FINLINE bool operator==(const BasicString<T, Alloc>& lhs, typename BasicString<T, Alloc>::ConstPointer rhs)
{
	return lhs.Compare(rhs) == 0;
}

template<typename T, typename Alloc>
FINLINE bool operator==(typename BasicString<T, Alloc>::ConstPointer lhs, const BasicString<T, Alloc>& rhs)
{
	return rhs.Compare(lhs) == 0;
}

template<typename T, typename Alloc>
FINLINE bool operator!=(const BasicString<T, Alloc>& lhs, const BasicString<T, Alloc>& rhs)
{
	return !(lhs == rhs);
}

template<typename T, typename Alloc>
FINLINE bool operator!=(const BasicString<T, Alloc>& lhs, typename BasicString<T, Alloc>::ConstPointer rhs)
{
	return !(lhs == rhs);
}

template<typename T, typename Alloc>
FINLINE bool operator!=(typename BasicString<T, Alloc>::ConstPointer lhs, const BasicString<T, Alloc>& rhs)
{
	return !(lhs == rhs);
}

template<typename T, typename Alloc>
FINLINE bool operator<(const BasicString<T, Alloc>& lhs, const BasicString<T, Alloc>& rhs)
{
	return lhs.Compare(rhs) < 0;
}

template<typename T, typename Alloc>
FINLINE bool operator<(const BasicString<T, Alloc>& lhs, typename BasicString<T, Alloc>::ConstPointer rhs)
{
	return lhs.Compare(rhs) < 0;
}

template<typename T, typename Alloc>
FINLINE bool operator<(typename BasicString<T, Alloc>::ConstPointer lhs, const BasicString<T, Alloc>& rhs)
{
	return rhs.Compare(lhs) > 0;
}

template<typename T, typename Alloc>
FINLINE bool operator>(const BasicString<T, Alloc>& lhs, const BasicString<T, Alloc>& rhs)
{
	return rhs < lhs;
}

template<typename T, typename Alloc>
FINLINE bool operator>(const BasicString<T, Alloc>& lhs, typename BasicString<T, Alloc>::ConstPointer rhs)
{
	return rhs < lhs;
}

template<typename T, typename Alloc>
FINLINE bool operator>(typename BasicString<T, Alloc>::ConstPointer lhs, const BasicString<T, Alloc>& rhs)
{
	return rhs < lhs;
}

template<typename T, typename Alloc>
FINLINE bool operator<=(const BasicString<T, Alloc>& lhs, const BasicString<T, Alloc>& rhs)
{
	return !(rhs < lhs);
}

template<typename T, typename Alloc>
FINLINE bool operator<=(const BasicString<T, Alloc>& lhs, typename BasicString<T, Alloc>::ConstPointer rhs)
{
	return !(rhs < lhs);
}

template<typename T, typename Alloc>
FINLINE bool operator<=(typename BasicString<T, Alloc>::ConstPointer lhs, const BasicString<T, Alloc>& rhs)
{
	return !(rhs < lhs);
}

template<typename T, typename Alloc>
FINLINE bool operator>=(const BasicString<T, Alloc>& lhs, const BasicString<T, Alloc>& rhs)
{
	return !(lhs < rhs);
}

template<typename T, typename Alloc>
FINLINE bool operator>=(const BasicString<T, Alloc>& lhs, typename BasicString<T, Alloc>::ConstPointer rhs)
{
	return !(lhs < rhs);
}

template<typename T, typename Alloc>
FINLINE bool operator>=(typename BasicString<T, Alloc>::ConstPointer lhs, const BasicString<T, Alloc>& rhs)
{
	return !(lhs < rhs);
}

using String = BasicString<char>;
using WString = BasicString<wchar_t>;


template<>
struct Hash<String>
{
	constexpr static bool c_HasHashImpl = true;

	static std::size_t hash(const String& string)
	{
		std::size_t hash = 0;
		for (auto it = string.begin(); it != string.end(); ++it)
		{
			hash = *it + (hash << 6) + (hash << 16) - hash;
		}
		return hash;
	}
};


template<>
struct StringConverter<char>
{
	constexpr static bool c_HasConverter = true;

	template<typename CharType, typename Alloc>
	static void Append(BasicString<CharType, Alloc>& string, const char& ch) {
		string.Append(&ch, &ch + 1);
	}
};

template<>
struct StringConverter<float>
{
	constexpr static bool c_HasConverter = true;

	template<typename CharType, typename Alloc>
	static void Append(BasicString<CharType, Alloc>& string, const float& value) {
		char buf[50];
		int len = snprintf(buf, 50, "%f", value);
		string.Append(buf, buf+len);
	}

};