#pragma once

#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include <limits>

#include "Common.hpp"


/*
 * It is possible that every realloc command will need to clear new added memory (if any) to zero,
 * if it's going to make problems.
 */

AX_NAMESPACE

template<typename T>
struct ArrayIteratorBase
{
	T* Ptr;
};

template<typename T>
struct ArrayIterator : ArrayIteratorBase<T>
{
	FINLINE T& operator*() const noexcept
	{
		return *this->Ptr;
	}

	FINLINE T* operator->() const noexcept
	{
		return this->Ptr;
	}

	FINLINE ArrayIterator& operator++()
	{
		this->Ptr++;
		return *this;
	}

	FINLINE ArrayIterator operator++(int)
	{
		ArrayIterator tmp = *this;
		++(*this);
		return tmp;
	}

	FINLINE bool operator==(const ArrayIterator<T>& other) const
	{
		return this->Ptr == other.Ptr;
	}

	FINLINE bool operator!=(const ArrayIterator<T>& other) const
	{
		return this->Ptr != other.Ptr;
	}

	FINLINE ArrayIterator operator+(uint32_t index) const
	{
		return {this->Ptr + index};
	}

	FINLINE ArrayIterator& operator+=(uint32_t index) const
	{
		this->Ptr += index;
		return *this;
	}
};

template<typename T>
struct ArrayConstIterator : ArrayIteratorBase<T>
{
	FINLINE T& operator*() const noexcept
	{
		return *this->Ptr;
	}

	FINLINE T& operator->() const noexcept
	{
		return *this->Ptr;
	}

	FINLINE const ArrayConstIterator& operator++()
	{
		this->Ptr++;
		return *this;
	}

	FINLINE ArrayConstIterator operator++(int)
	{
		ArrayConstIterator tmp = *this;
		++(*this);
		return tmp;
	}

	FINLINE bool operator==(const ArrayConstIterator<T>& other) const
	{
		return this->Ptr == other.Ptr;
	}

	FINLINE bool operator!=(const ArrayConstIterator<T>& other) const
	{
		return this->Ptr != other.Ptr;
	}

	FINLINE ArrayConstIterator operator+(uint32_t index) const
	{
		return {this->Ptr + index};
	}

	FINLINE ArrayConstIterator& operator+=(uint32_t index) const
	{
		this->Ptr += index;
		return *this;
	}
};

template<typename T>
class Array
{
private:
	T* m_Data = nullptr;
	uint32_t m_Size = 0;
	uint32_t m_Capacity = 0;
public:
	using iterator_base = ArrayIteratorBase<T>;
	using iterator = ArrayIterator<T>;
	using const_iterator = ArrayConstIterator<T>;
	using size_type = uint32_t;

	FINLINE Array(uint32_t defaultSize = 12) : m_Data(static_cast<T*>(calloc(1, sizeof(T) * defaultSize))), m_Size(0), m_Capacity(defaultSize)
	{}

	FINLINE ~Array()
	{ if (m_Data != nullptr) free(m_Data); }

	FINLINE Array(const Array& other) : m_Data(), m_Size(other.m_Size), m_Capacity(other.m_Capacity)
	{
		m_Data = static_cast<T*>(calloc(1, sizeof(T) * other.m_Capacity));

		memcpy(m_Data, other.m_Data, other.DataSize());
	}

	/*FINLINE Array(Array&& other) noexcept: m_Data(other.m_Data), m_Size(other.m_Size), m_Capacity(other.m_Capacity)
	{
		other.m_Data = nullptr;
	}*/

	Array& operator=(const Array& other)
	{
		m_Capacity = other.m_Capacity;
		m_Size = other.m_Size;

		if (m_Data != nullptr)
		{
			m_Data = static_cast<T*>(realloc(m_Data, other.m_Capacity * sizeof(T)));
		}
		else
		{
			m_Data = static_cast<T*>(calloc(1, other.m_Capacity * sizeof(T)));
		}

		memcpy(m_Data, other.m_Data, other.DataSize());
		return *this;
	}

	FINLINE iterator begin()
	{ return iterator{m_Data}; }

	FINLINE iterator end()
	{ return iterator{m_Data + m_Size}; }

	FINLINE const_iterator begin() const
	{ return const_iterator{m_Data}; }

	FINLINE const_iterator end() const
	{ return const_iterator{m_Data + m_Size}; }

	[[nodiscard]] FINLINE bool Any() const
	{ return m_Size > 0; }

	[[nodiscard]] FINLINE bool Empty() const
	{ return m_Size == 0; }

	FINLINE T& operator[](int index)
	{
		return m_Data[index];
	}

	FINLINE T& operator[](uint32_t index)
	{
		return m_Data[index];
	}

	FINLINE T& Add(T type)
	{
		if (m_Size + 1 > m_Capacity)
		{
			m_Capacity = CalculateGrowth(m_Size + 1);
			m_Data = static_cast<T*>(realloc(m_Data, sizeof(T) * m_Capacity));
		}

		return (m_Data[m_Size++] = type);
	}

	FINLINE void AddRange(T* begin, T* end)
	{
		uint64 len = uint64(end - begin) / sizeof(T);
		
		if (m_Size + len - 1 > m_Capacity)
		{
			m_Capacity = CalculateGrowth(m_Size + len);
			m_Data = static_cast<T*>(realloc(m_Data, sizeof(T) * m_Capacity));
		}

		while (begin != end) 
		{
			m_Data[m_Size++] = *begin++;
		}
	}

	FINLINE void SetRange(int start, int end, T value) 
	{
		for (; start < end; ++start) m_Data[start] = value;
	}

	FINLINE T& Emplace(T&& type)
	{
		if (m_Size + 1 > m_Capacity)
		{
			m_Capacity = CalculateGrowth(m_Size + 1);
			m_Data = static_cast<T*>(realloc(m_Data, sizeof(T) * m_Capacity));
		}

		return (m_Data[m_Size++] = std::move(type));
	}

	FINLINE void InsertAt(uint32_t index, T type)
	{
		ax_assert(index > m_Size);

		if (index > m_Size)
			return;

		if (m_Size + 1 > m_Capacity)
		{
			m_Capacity = CalculateGrowth(m_Size + 1);
			m_Data = static_cast<T*>(realloc(m_Data, sizeof(T) * m_Capacity));
		}

		memmove(m_Data + index + 1, m_Data + index, (m_Size - index) * sizeof(T));
		m_Size++;

		m_Data[index] = type;
	}

	FINLINE void EmplaceAt(uint32_t index, T&& type)
	{
		ax_assert(index > m_Size);

		if (index > m_Size)
			return;

		if (m_Size + 1 > m_Capacity)
		{
			m_Capacity = CalculateGrowth(m_Size + 1);
			m_Data = static_cast<T*>(realloc(m_Data, sizeof(T) * m_Capacity));
		}

		memmove(m_Data + index + 1, m_Data + index, (m_Size - index) * sizeof(T));
		m_Size++;

		m_Data[index] = std::move(type);
	}

	FINLINE void Insert(const Array& other)
	{
		if (other.m_Size > m_Capacity)
		{
			m_Capacity += other.m_Capacity;
			m_Data = static_cast<T*>(realloc(m_Data, sizeof(T) * m_Capacity));
		}

		memcpy(m_Data + m_Size, other.m_Data, other.DataSize());

		m_Size += other.m_Size;
	}

	// <summary> returns number of elements removed </summary>
	int RemoveAll(const T& what)
	{
		int freeIndex = 0;   // the first free slot in items array

		// Find the first item which needs to be removed.
		while (freeIndex < m_Size && m_Data[freeIndex] != what) freeIndex++;
		if (freeIndex >= m_Size) return 0;

		int current = freeIndex + 1;
		while (current < m_Size)
		{
			// Find the first item which needs to be kept.
			while (current < m_Size && m_Data[current] == what) current++;

			if (current < m_Size)
			{
				// copy item to the free slot.
				m_Data[freeIndex++] = m_Data[current++];
			}
		}

		int numCleared = m_Size - freeIndex;
		memset(m_Data + freeIndex, 0, numCleared * sizeof(T));
		m_Size = freeIndex;

		return numCleared; // removed item count
	}

	// <summary> returns number of elements removed </summary>
	int RemoveAll(bool(* match)(const T&))
	{
		int freeIndex = 0;   // the first free slot in items array

		// Find the first item which needs to be removed.
		while (freeIndex < m_Size && match(m_Data[freeIndex]) == false) freeIndex++;
		if (freeIndex >= m_Size) return 0;

		int current = freeIndex + 1;
		while (current < m_Size)
		{
			// Find the first item which needs to be kept.
			while (current < m_Size && match(m_Data[current]) == true) current++;

			if (current < m_Size)
			{
				// copy item to the free slot.
				m_Data[freeIndex++] = m_Data[current++];
			}
		}

		int numCleared = m_Size - freeIndex;
		memset(m_Data + freeIndex, 0, numCleared * sizeof(T));
		m_Size = freeIndex;

		return numCleared; // removed item count
	}

	FINLINE bool RemoveAt(uint32_t index)
	{
		if (index >= m_Size)
		{
			return false;
		}

		memmove(m_Data + index, m_Data + index + 1, (m_Size - index - 1) * sizeof(T));
		m_Size--;

		return true;
	}

	FINLINE void Remove(T value)
	{
		for (uint32_t i = 0; i < m_Size; i++)
		{
			if (m_Data[i] == value)
			{
				memmove(m_Data + i, m_Data + i + 1, (m_Size - i - 1) * sizeof(T));
				m_Size--;
				//i--;
				return;
			}
		}
	}

	FINLINE void Remove(bool(* match)(const T&))
	{
		for (uint32_t i = 0; i < m_Size; i++)
		{
			if (match(m_Data[i]) == true)
			{
				memmove(m_Data + i, m_Data + i + 1, (m_Size - i - 1) * sizeof(T));
				m_Size--;
				//i--;
				return;
			}
		}
	}

	FINLINE void Remove(const iterator_base& it)
	{
		int32_t index = it.Ptr - m_Data;

		ax_assert(index >= 0 && index < m_Size);

		if (index >= 0 && index < m_Size)
		{
			RemoveAt(uint32_t(index));
		}
	}

	FINLINE T& At(uint32_t index)
	{
		return m_Data[index];
	}

	[[nodiscard]] FINLINE size_type Size() const
	{ return m_Size; }

	[[nodiscard]] FINLINE size_type DataSize() const
	{ return m_Size * sizeof(T); }

	[[nodiscard]] FINLINE size_type Capacity() const
	{ return m_Capacity; }

	FINLINE Array& operator+=(T type)
	{
		Add(type);
		return *this;
	}

	FINLINE Array& operator-=(T type)
	{
		Remove(type);
		return *this;
	}
public:
	
	FINLINE static void SetRange(T* start, T* end, T value) {
		for (; start < end; ++start) *start = value;
	}

private:
	// Same as std::vector
	[[nodiscard]] FINLINE size_type CalculateGrowth(size_type newSize) const
	{
		const size_type old_capacity = Capacity();
		const auto max = std::numeric_limits<size_type>::max();

		if (old_capacity > max - old_capacity / 2) {
			return max; // geometric growth would overflow
		}

		const size_type geometric_size = old_capacity + old_capacity / 2;

		if (geometric_size < newSize) {
			return newSize; // geometric growth would be insufficient
		}

		return geometric_size; // geometric growth is sufficient
	}
};

AX_END_NAMESPACE