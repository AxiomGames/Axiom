#pragma once

/*-
 * this file is based on: https://github.com/mendsley/tinystl
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

#include "Common.hpp"
#include "New.hpp"
#include "Buffer.hpp"
#include "Memory.hpp"

template<typename T, typename Alloc = DEFAULT_ALLOCATOR>
class Array
{
public:
	typedef const T* ConstIterator;
	typedef T ValueType;
	typedef T* Iterator;

	Array();

	Array(const Array& other);

	Array(Array&& other);

	Array(size_t size);

	Array(size_t size, const T& value);

	Array(const T* first, const T* last);

	~Array();

	Array& operator=(const Array& other);

	Array& operator=(Array&& other);

	void Assign(const T* first, const T* last);

	const T* Data() const;

	T* Data();

	size_t Count() const;

	size_t Capacity() const;

	bool Empty() const;

	T& operator[](size_t idx);

	const T& operator[](size_t idx) const;

	const T& Front() const;

	T& Front();

	const T& Back() const;

	T& Back();

	void Resize(size_t size);

	void Resize(size_t size, const T& value);

	void Clear();

	void Reserve(size_t capacity);

	void PushBack(const T& t);

	void PopBack();

	void EmplaceBack();

	template<typename Param>
	T& EmplaceBack(const Param& param);

	void ShrinkToFit();

	void Swap(Array& other);

	Iterator begin();

	Iterator end();

	ConstIterator begin() const;

	ConstIterator end() const;

	void Insert(Iterator where);

	void Insert(Iterator where, const T& value);

	void Insert(Iterator where, const T* first, const T* last);

	template<typename Param>
	void Emplace(Iterator where, const Param& param);

	Iterator Erase(Iterator where);

	Iterator Erase(Iterator first, Iterator last);

	Iterator EraseUnordered(Iterator where);

	Iterator EraseUnordered(Iterator first, Iterator last);

	T& Add(const T& t);

	void EmplaceAt(size_t where, const T& value);

	Iterator Remove(Iterator where);

private:
	AxSTL::Buffer<T, Alloc> m_Buffer{};
};

template<typename T, typename Alloc>
FINLINE Array<T, Alloc>::Array()
{
	AxSTL::buffer_init(&m_Buffer);
}

template<typename T, typename Alloc>
FINLINE Array<T, Alloc>::Array(const Array& other)
{
	AxSTL::buffer_init(&m_Buffer);
	AxSTL::buffer_reserve(&m_Buffer, other.Count());
	AxSTL::buffer_insert(&m_Buffer, m_Buffer.last, other.m_Buffer.first, other.m_Buffer.last);
}

template<typename T, typename Alloc>
FINLINE Array<T, Alloc>::Array(Array&& other)
{
	AxSTL::buffer_move(&m_Buffer, &other.m_Buffer);
}

template<typename T, typename Alloc>
FINLINE Array<T, Alloc>::Array(size_t size)
{
	AxSTL::buffer_init(&m_Buffer);
	AxSTL::buffer_resize(&m_Buffer, size);
}

template<typename T, typename Alloc>
FINLINE Array<T, Alloc>::Array(size_t size, const T& value)
{
	AxSTL::buffer_init(&m_Buffer);
	AxSTL::buffer_resize(&m_Buffer, size, value);
}

template<typename T, typename Alloc>
FINLINE Array<T, Alloc>::Array(const T* first, const T* last)
{
	AxSTL::buffer_init(&m_Buffer);
	AxSTL::buffer_insert(&m_Buffer, m_Buffer.last, first, last);
}

template<typename T, typename Alloc>
FINLINE Array<T, Alloc>::~Array()
{
	AxSTL::buffer_destroy(&m_Buffer);
}

template<typename T, typename Alloc>
FINLINE Array<T, Alloc>& Array<T, Alloc>::operator=(const Array& other)
{
	Array(other).Swap(*this);
	return *this;
}

template<typename T, typename Alloc>
Array<T, Alloc>& Array<T, Alloc>::operator=(Array&& other)
{
	AxSTL::buffer_destroy(&m_Buffer);
	AxSTL::buffer_move(&m_Buffer, &other.m_Buffer);
	return *this;
}

template<typename T, typename Alloc>
FINLINE void Array<T, Alloc>::Assign(const T* first, const T* last)
{
	AxSTL::buffer_clear(&m_Buffer);
	AxSTL::buffer_insert(&m_Buffer, m_Buffer.last, first, last);
}

template<typename T, typename Alloc>
FINLINE const T* Array<T, Alloc>::Data() const
{
	return m_Buffer.first;
}

template<typename T, typename Alloc>
FINLINE T* Array<T, Alloc>::Data()
{
	return m_Buffer.first;
}

template<typename T, typename Alloc>
FINLINE size_t Array<T, Alloc>::Count() const
{
	return (size_t) (m_Buffer.last - m_Buffer.first);
}

template<typename T, typename Alloc>
FINLINE size_t Array<T, Alloc>::Capacity() const
{
	return (size_t) (m_Buffer.capacity - m_Buffer.first);
}

template<typename T, typename Alloc>
FINLINE bool Array<T, Alloc>::Empty() const
{
	return m_Buffer.last == m_Buffer.first;
}

template<typename T, typename Alloc>
FINLINE T& Array<T, Alloc>::operator[](size_t idx)
{
	return m_Buffer.first[idx];
}

template<typename T, typename Alloc>
FINLINE const T& Array<T, Alloc>::operator[](size_t idx) const
{
	return m_Buffer.first[idx];
}

template<typename T, typename Alloc>
FINLINE const T& Array<T, Alloc>::Front() const
{
	return m_Buffer.first[0];
}

template<typename T, typename Alloc>
FINLINE T& Array<T, Alloc>::Front()
{
	return m_Buffer.first[0];
}

template<typename T, typename Alloc>
FINLINE const T& Array<T, Alloc>::Back() const
{
	return m_Buffer.last[-1];
}

template<typename T, typename Alloc>
FINLINE T& Array<T, Alloc>::Back()
{
	return m_Buffer.last[-1];
}

template<typename T, typename Alloc>
FINLINE void Array<T, Alloc>::Resize(size_t size)
{
	AxSTL::buffer_resize(&m_Buffer, size);
}

template<typename T, typename Alloc>
FINLINE void Array<T, Alloc>::Resize(size_t size, const T& value)
{
	AxSTL::buffer_resize(&m_Buffer, size, value);
}

template<typename T, typename Alloc>
FINLINE void Array<T, Alloc>::Clear()
{
	AxSTL::buffer_clear(&m_Buffer);
}

template<typename T, typename Alloc>
FINLINE void Array<T, Alloc>::Reserve(size_t capacity)
{
	AxSTL::buffer_reserve(&m_Buffer, capacity);
}

template<typename T, typename Alloc>
FINLINE void Array<T, Alloc>::PushBack(const T& t)
{
	AxSTL::buffer_append(&m_Buffer, &t);
}

template<typename T, typename Alloc>
FINLINE void Array<T, Alloc>::EmplaceBack()
{
	AxSTL::buffer_append(&m_Buffer);
}

template<typename T, typename Alloc>
template<typename Param>
FINLINE T& Array<T, Alloc>::EmplaceBack(const Param& param)
{
	return *AxSTL::buffer_emplace(&m_Buffer, &param);
}

template<typename T, typename Alloc>
FINLINE void Array<T, Alloc>::PopBack()
{
	AxSTL::buffer_erase(&m_Buffer, m_Buffer.last - 1, m_Buffer.last);
}

template<typename T, typename Alloc>
FINLINE void Array<T, Alloc>::ShrinkToFit()
{
	AxSTL::buffer_shrink_to_fit(&m_Buffer);
}

template<typename T, typename Alloc>
FINLINE void Array<T, Alloc>::Swap(Array& other)
{
	AxSTL::buffer_swap(&m_Buffer, &other.m_Buffer);
}

template<typename T, typename Alloc>
FINLINE typename Array<T, Alloc>::Iterator Array<T, Alloc>::begin()
{
	return m_Buffer.first;
}

template<typename T, typename Alloc>
FINLINE typename Array<T, Alloc>::Iterator Array<T, Alloc>::end()
{
	return m_Buffer.last;
}

template<typename T, typename Alloc>
FINLINE typename Array<T, Alloc>::ConstIterator Array<T, Alloc>::begin() const
{
	return m_Buffer.first;
}

template<typename T, typename Alloc>
FINLINE typename Array<T, Alloc>::ConstIterator Array<T, Alloc>::end() const
{
	return m_Buffer.last;
}

template<typename T, typename Alloc>
FINLINE void Array<T, Alloc>::Insert(typename Array::Iterator where)
{
	AxSTL::buffer_insert(&m_Buffer, where, 1);
}

template<typename T, typename Alloc>
FINLINE void Array<T, Alloc>::Insert(Iterator where, const T& value)
{
	AxSTL::buffer_insert(&m_Buffer, where, &value, &value + 1);
}

template<typename T, typename Alloc>
FINLINE void Array<T, Alloc>::Insert(Iterator where, const T* first, const T* last)
{
	AxSTL::buffer_insert(&m_Buffer, where, first, last);
}

template<typename T, typename Alloc>
FINLINE typename Array<T, Alloc>::Iterator Array<T, Alloc>::Erase(Iterator where)
{
	return AxSTL::buffer_erase(&m_Buffer, where, where + 1);
}

template<typename T, typename Alloc>
FINLINE typename Array<T, Alloc>::Iterator Array<T, Alloc>::Erase(Iterator first, Iterator last)
{
	return AxSTL::buffer_erase(&m_Buffer, first, last);
}

template<typename T, typename Alloc>
FINLINE typename Array<T, Alloc>::Iterator Array<T, Alloc>::EraseUnordered(Iterator where)
{
	return AxSTL::buffer_erase_unordered(&m_Buffer, where, where + 1);
}

template<typename T, typename Alloc>
FINLINE typename Array<T, Alloc>::Iterator Array<T, Alloc>::EraseUnordered(Iterator first, Iterator last)
{
	return AxSTL::buffer_erase_unordered(&m_Buffer, first, last);
}

template<typename T, typename Alloc>
template<typename Param>
void Array<T, Alloc>::Emplace(typename Array::Iterator where, const Param& param)
{
	AxSTL::buffer_insert(&m_Buffer, where, &param, &param + 1);
}

template<typename T, typename Alloc>
FINLINE T& Array<T, Alloc>::Add(const T& t)
{
	return *AxSTL::buffer_emplace(&m_Buffer, t);
}

template<typename T, typename Alloc>
FINLINE void Array<T, Alloc>::EmplaceAt(size_t where, const T& value)
{
	AxSTL::buffer_insert(&m_Buffer, begin() + where, &value, &value + 1);
}

template<typename T, typename Alloc>
FINLINE typename Array<T, Alloc>::Iterator Array<T, Alloc>::Remove(Array::Iterator where)
{
	return Erase(where);
}
