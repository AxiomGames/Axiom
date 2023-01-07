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

	for (const char c : view)
	{
		hash = ((hash << 5) + hash) + c;
	}

	return hash;
}

StrHashID constexpr operator "" _HASH(const char* s, size_t)
{
	return HashDjb2(s);
}


template<typename T, typename Alloc = DEFAULT_ALLOCATOR>
class BasicString
{
public:
	BasicString();

	BasicString(const BasicString& other);

	BasicString(const char* sz);

	BasicString(const char* first, const char* last);

	BasicString(const char* sz, size_t len);

	~BasicString();

	BasicString& operator=(const BasicString& other);

	BasicString& operator=(char ch);

	BasicString& operator=(const char* sz);

	BasicString& operator+=(const BasicString& other);

	BasicString& operator+=(char ch);

	BasicString& operator+=(const char* sz);

	const char* c_str() const;

	bool empty() const;

	size_t size() const;

	size_t capacity() const;

	typedef char* iterator;

	iterator begin();

	iterator end();

	typedef const char* const_iterator;

	const_iterator begin() const;

	const_iterator end() const;

	char operator[](size_t pos) const;

	char& operator[](size_t pos);

	int compare(const BasicString& other) const;

	int compare(const char* sz) const;

	void reserve(size_t capacity);

	void resize(size_t n);

	void resize(size_t n, char ch);

	void clear();

	void assign(char ch);

	void assign(const char* sz);

	void assign(const char* first, const char* last);

	void assign(const BasicString& other);

	void push_back(char ch);

	void append(const char* sz);

	void append(const char* first, const char* last);

	void append(const BasicString& other);

	void insert(iterator where, char ch);

	void insert(iterator where, const char* sz);

	void insert(iterator where, const char* first, const char* last);

	void insert(iterator where, const BasicString& other);

	void erase(iterator first, iterator last);

	void swap(BasicString& other);

private:
	typedef char* pointer;
	static const size_t c_nbuffer = 16;
	static const size_t c_longflag = ((size_t) 1) << (sizeof(size_t) * 8 - 1);
	size_t m_size;
	union
	{
		struct
		{
			pointer m_first;
			pointer m_capacity;
		};
		char m_buffer[c_nbuffer];
	};
};

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>::BasicString()
	: m_size(0)
{
	m_buffer[0] = 0;
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>::BasicString(const BasicString& other)
	: m_size(0)
{
	assign(other);
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>::BasicString(const char* sz)
	: m_size(0)
{
	assign(sz);
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>::BasicString(const char* first, const char* last)
	: m_size(0)
{
	assign(first, last);
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>::BasicString(const char* sz, size_t len)
	: m_size(0)
{
	append(sz, sz + len);
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>::~BasicString()
{
	if (m_size & c_longflag)
		DEFAULT_ALLOCATOR::Free(m_first);
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>& BasicString<T, Alloc>::operator=(const BasicString& other)
{
	if (this != &other)
		assign(other);
	return *this;
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>& BasicString<T, Alloc>::operator=(char ch)
{
	assign(ch);
	return *this;
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>& BasicString<T, Alloc>::operator=(const char* sz)
{
	assign(sz);
	return *this;
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>& BasicString<T, Alloc>::operator+=(const BasicString& other)
{
	append(other);
	return *this;
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>& BasicString<T, Alloc>::operator+=(char ch)
{
	push_back(ch);
	return *this;
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc>& BasicString<T, Alloc>::operator+=(const char* sz)
{
	append(sz);
	return *this;
}

template<typename T, typename Alloc>
FINLINE const char* BasicString<T, Alloc>::c_str() const
{
	if (m_size & c_longflag)
		return m_first;
	else
		return m_buffer;
}

template<typename T, typename Alloc>
FINLINE bool BasicString<T, Alloc>::empty() const
{
	return size() == 0;
}

template<typename T, typename Alloc>
FINLINE size_t BasicString<T, Alloc>::size() const
{
	return m_size & ~c_longflag;
}

template<typename T, typename Alloc>
FINLINE size_t BasicString<T, Alloc>::capacity() const
{
	if (m_size & c_longflag)
		return m_capacity - m_first - 1;
	else
		return c_nbuffer - 1;
}

template<typename T, typename Alloc>
FINLINE typename BasicString<T, Alloc>::iterator BasicString<T, Alloc>::begin()
{
	if (m_size & c_longflag)
		return m_first;
	else
		return m_buffer;
}

template<typename T, typename Alloc>
FINLINE typename BasicString<T, Alloc>::iterator BasicString<T, Alloc>::end()
{
	if (m_size & c_longflag)
		return m_first + (m_size & ~c_longflag);
	else
		return m_buffer + m_size;
}

template<typename T, typename Alloc>
FINLINE typename BasicString<T, Alloc>::const_iterator BasicString<T, Alloc>::begin() const
{
	if (m_size & c_longflag)
		return m_first;
	else
		return m_buffer;
}

template<typename T, typename Alloc>
FINLINE typename BasicString<T, Alloc>::const_iterator BasicString<T, Alloc>::end() const
{
	if (m_size & c_longflag)
		return m_first + (m_size & ~c_longflag);
	else
		return m_buffer + m_size;
}

template<typename T, typename Alloc>
FINLINE char BasicString<T, Alloc>::operator[](size_t pos) const
{
	return begin()[pos];
}

template<typename T, typename Alloc>
FINLINE char& BasicString<T, Alloc>::operator[](size_t pos)
{
	return begin()[pos];
}

template<typename T, typename Alloc>
FINLINE int BasicString<T, Alloc>::compare(const BasicString& other) const
{
	return compare(other.c_str());
}

template<typename T, typename Alloc>
FINLINE int BasicString<T, Alloc>::compare(const char* sz) const
{
	const char* it = c_str();
	for (; *it && *sz && (*it == *sz); ++it, ++sz);
	return *it - *sz;
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::reserve(size_t cap)
{
	if (cap <= capacity())
		return;

	pointer newfirst = (pointer) Alloc::Malloc(cap + 1);
	for (pointer it = begin(), newit = newfirst, e = end() + 1; it != e; ++it, ++newit)
		*newit = *it;
	if (m_size & c_longflag)
		Alloc::Free(m_first);
	else
		m_size |= c_longflag;
	m_first = newfirst;
	m_capacity = m_first + cap + 1;
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::resize(size_t n)
{
	resize(n, 0);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::resize(size_t n, char ch)
{
	if (size() < n)
	{
		reserve(n);
		for (pointer it = end(), e = begin() + n; it != e; ++it)
			*it = ch;
	}
	pointer it = begin() + n;
	*it = 0;
	m_size = n | (m_size & c_longflag);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::clear()
{
	resize(0);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::assign(char ch)
{
	pointer it = begin();
	*it = ch;
	*(it + 1) = 0;
	m_size = 1 | (m_size & c_longflag);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::assign(const char* sz)
{
	size_t len = 0;
	for (const char* it = sz; *it; ++it)
		++len;

	assign(sz, sz + len);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::assign(const char* first, const char* last)
{
	size_t newsize = last - first;
	reserve(newsize);

	pointer newit = begin();
	for (const char* it = first; it != last; ++it, ++newit)
		*newit = *it;
	*newit = 0;
	m_size = newsize | (m_size & c_longflag);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::assign(const BasicString& other)
{
	assign(other.begin(), other.end());
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::push_back(char ch)
{
	if (size() != capacity())
	{
		pointer it = end();
		*it = ch;
		*(it + 1) = 0;
		++m_size;
	}
	else
	{
		append(&ch, &ch + 1);
	}
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::append(const char* sz)
{
	size_t len = 0;
	for (const char* it = sz; *it; ++it)
		++len;
	append(sz, sz + len);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::append(const char* first, const char* last)
{
	const size_t newsize = (size_t) (size() + (last - first));
	if (newsize > capacity())
		reserve((newsize * 3) / 2);

	pointer newit = end();
	for (const char* it = first; it != last; ++it, ++newit)
		*newit = *it;
	*newit = 0;
	m_size = newsize | (m_size & c_longflag);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::append(const BasicString& other)
{
	append(other.begin(), other.end());
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::insert(iterator where, char ch)
{
	insert(where, &ch, &ch + 1);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::insert(iterator where, const char* sz)
{
	size_t len = 0;
	for (const char* it = sz; *it; ++it)
		++len;
	insert(where, sz, sz + len);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::insert(iterator where, const char* first, const char* last)
{
	if (first == last)
		return;

	const size_t w = where - begin();
	const size_t newsize = (size_t) (size() + (last - first));
	if (newsize > capacity())
		reserve((newsize * 3) / 2);

	pointer newit = begin() + newsize;
	for (pointer it = end(), e = begin() + w; it >= e; --it, --newit)
		*newit = *it;

	newit = begin() + w;
	for (const char* it = first; it != last; ++it, ++newit)
		*newit = *it;
	m_size = newsize | (m_size & c_longflag);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::insert(iterator where, const BasicString& other)
{
	insert(where, other.begin(), other.end());
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::erase(iterator first, iterator last)
{
	if (first == last)
		return;

	const size_t newsize = (size_t) (size() + (first - last));
	pointer newit = first;
	for (pointer it = last, e = end(); it != e; ++it, ++newit)
		*newit = *it;
	*newit = 0;
	m_size = newsize | (m_size & c_longflag);
}

template<typename T, typename Alloc>
FINLINE void BasicString<T, Alloc>::swap(BasicString& other)
{
	const size_t tsize = m_size;
	pointer tfirst, tcapacity;
	char tbuffer[c_nbuffer];

	if (tsize & c_longflag)
	{
		tfirst = m_first;
		tcapacity = m_capacity;
	}
	else
	{
		for (pointer it = m_buffer, newit = tbuffer, e = m_buffer + tsize + 1; it != e; ++it, ++newit)
			*newit = *it;
	}

	m_size = other.m_size;
	if (other.m_size & c_longflag)
	{
		m_first = other.m_first;
		m_capacity = other.m_capacity;
	}
	else
	{
		for (pointer it = other.m_buffer, newit = m_buffer, e = other.m_buffer + m_size + 1; it != e; ++it, ++newit)
			*newit = *it;
	}

	other.m_size = tsize;
	if (tsize & c_longflag)
	{
		other.m_first = tfirst;
		other.m_capacity = tcapacity;
	}
	else
	{
		for (pointer it = tbuffer, newit = other.m_buffer, e = tbuffer + tsize + 1; it != e; ++it, ++newit)
			*newit = *it;
	}
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc> operator+(const BasicString<T, Alloc>& lhs, const BasicString<T, Alloc>& rhs)
{
	BasicString<T, Alloc> ret;
	ret.reserve(lhs.size() + rhs.size());
	ret += lhs;
	ret += rhs;
	return ret;
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc> operator+(const BasicString<T, Alloc>& lhs, const char* rhs)
{
	BasicString ret = lhs;
	ret += rhs;
	return ret;
}

template<typename T, typename Alloc>
FINLINE BasicString<T, Alloc> operator+(const char* lhs, const BasicString<T, Alloc>& rhs)
{
	BasicString<T, Alloc> ret = lhs;
	ret += rhs;
	return ret;
}

template<typename T, typename Alloc>
FINLINE bool operator==(const BasicString<T, Alloc>& lhs, const BasicString<T, Alloc>& rhs)
{
	if (lhs.size() != rhs.size())
		return false;

	return lhs.compare(rhs) == 0;
}

template<typename T, typename Alloc>
FINLINE bool operator==(const BasicString<T, Alloc>& lhs, const char* rhs)
{
	return lhs.compare(rhs) == 0;
}

template<typename T, typename Alloc>
FINLINE bool operator==(const char* lhs, const BasicString<T, Alloc>& rhs)
{
	return rhs.compare(lhs) == 0;
}

template<typename T, typename Alloc>
FINLINE bool operator!=(const BasicString<T, Alloc>& lhs, const BasicString<T, Alloc>& rhs)
{
	return !(lhs == rhs);
}

template<typename T, typename Alloc>
FINLINE bool operator!=(const BasicString<T, Alloc>& lhs, const char* rhs)
{
	return !(lhs == rhs);
}

template<typename T, typename Alloc>
FINLINE bool operator!=(const char* lhs, const BasicString<T, Alloc>& rhs)
{
	return !(lhs == rhs);
}

template<typename T, typename Alloc>
FINLINE bool operator<(const BasicString<T, Alloc>& lhs, const BasicString<T, Alloc>& rhs)
{
	return lhs.compare(rhs) < 0;
}

template<typename T, typename Alloc>
FINLINE bool operator<(const BasicString<T, Alloc>& lhs, const char* rhs)
{
	return lhs.compare(rhs) < 0;
}

template<typename T, typename Alloc>
FINLINE bool operator<(const char* lhs, const BasicString<T, Alloc>& rhs)
{
	return rhs.compare(lhs) > 0;
}

template<typename T, typename Alloc>
FINLINE bool operator>(const BasicString<T, Alloc>& lhs, const BasicString<T, Alloc>& rhs)
{
	return rhs < lhs;
}

template<typename T, typename Alloc>
FINLINE bool operator>(const BasicString<T, Alloc>& lhs, const char* rhs)
{
	return rhs < lhs;
}

template<typename T, typename Alloc>
FINLINE bool operator>(const char* lhs, const BasicString<T, Alloc>& rhs)
{
	return rhs < lhs;
}

template<typename T, typename Alloc>
FINLINE bool operator<=(const BasicString<T, Alloc>& lhs, const BasicString<T, Alloc>& rhs)
{
	return !(rhs < lhs);
}

template<typename T, typename Alloc>
FINLINE bool operator<=(const BasicString<T, Alloc>& lhs, const char* rhs)
{
	return !(rhs < lhs);
}

template<typename T, typename Alloc>
FINLINE bool operator<=(const char* lhs, const BasicString<T, Alloc>& rhs)
{
	return !(rhs < lhs);
}

template<typename T, typename Alloc>
FINLINE bool operator>=(const BasicString<T, Alloc>& lhs, const BasicString<T, Alloc>& rhs)
{
	return !(lhs < rhs);
}

template<typename T, typename Alloc>
FINLINE bool operator>=(const BasicString<T, Alloc>& lhs, const char* rhs)
{
	return !(lhs < rhs);
}

template<typename T, typename Alloc>
FINLINE bool operator>=(const char* lhs, const BasicString<T, Alloc>& rhs)
{
	return !(lhs < rhs);
}

template<typename T, typename Alloc>
static FINLINE size_t hash(const BasicString<T, Alloc>& value)
{
	return hash_String(value.c_str(), value.size());
}

using String = BasicString<char>;