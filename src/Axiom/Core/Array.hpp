#pragma once

#include <cstdlib>
#include <cstdint>
#include <algorithm>

namespace Ax
{
template<typename T>
class Array
{
private:
	static constexpr int CapacityBlockSize = 12;
private:
	T* m_Data;
	uint32_t m_Size;
	uint32_t m_Capacity;
public:
	__forceinline Array(uint32_t defaultSize = 12) : m_Data(static_cast<T*>(malloc(sizeof(T) * defaultSize))), m_Size(0), m_Capacity(defaultSize)
	{}

	~Array()
	{ if (m_Data != nullptr) free(m_Data); }

	__forceinline Array(const Array& other) : m_Data(static_cast<T*>(malloc(sizeof(T) * other.m_Capacity))), m_Size(other.m_Size), m_Capacity(other.m_Capacity)
	{
		memcpy(m_Data, other.m_Data, other.m_Size * sizeof(T));
	}

	__forceinline Array(Array&& other) noexcept: m_Data(other.m_Data), m_Size(other.m_Size), m_Capacity(other.m_Capacity)
	{
		other.m_Data = nullptr;
	}

	T* begin()
	{ return m_Data; }

	T* end()
	{ return m_Data + m_Size; }

	T* begin() const
	{ return m_Data; }

	T* end() const
	{ return m_Data + m_Size; }

	[[nodiscard]] __forceinline bool Any() const
	{ return m_Size > 0; }

	[[nodiscard]] __forceinline bool Empty() const
	{ return m_Size == 0; }

	__forceinline T& operator[](int index)
	{
		return m_Data[index];
	}

	__forceinline T& operator[](uint32_t index)
	{
		return m_Data[index];
	}

	__forceinline Array& Add(T type)
	{
		if (m_Size > m_Capacity)
		{
			m_Capacity += CapacityBlockSize;
			m_Data = static_cast<T*>(realloc(m_Data, sizeof(T) * m_Capacity));
		}

		m_Data[m_Size++] = type;

		return *this;
	}

	__forceinline Array& Emplace(T&& type)
	{
		if (m_Size > m_Capacity)
		{
			m_Capacity += CapacityBlockSize;
			m_Data = static_cast<T*>(realloc(m_Data, sizeof(T) * m_Capacity));
		}

		m_Data[m_Size++] = std::move(type);

		return *this;
	}

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

		memset(m_Data + freeIndex, 0, m_Size - freeIndex);
		int result = m_Size - freeIndex;
		m_Size = freeIndex;

		return result; // removed item count
	}

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

		memset(m_Data + freeIndex, 0, m_Size - freeIndex);
		int result = m_Size - freeIndex;
		m_Size = freeIndex;

		return result; // removed item count
	}

	__forceinline bool RemoveAt(uint32_t index)
	{
		if (index >= m_Size)
		{
			return false;
		}

		memmove(m_Data + index, m_Data + index + 1, (m_Size - index - 1) * sizeof(T));
		m_Size--;

		return true;
	}

	__forceinline void Remove(T value)
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

	__forceinline void Remove(bool(* match)(const T&))
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

	__forceinline T& At(uint32_t index)
	{
		return m_Data[index];
	}

	[[nodiscard]] __forceinline uint32_t Size() const
	{ return m_Size; }

	[[nodiscard]] __forceinline uint32_t DataSize() const
	{ return m_Size * sizeof(T); }

	__forceinline Array& operator+=(T type)
	{
		Add(type);
		return *this;
	}

	__forceinline Array& operator-=(T type)
	{
		Remove(type);
		return *this;
	}
};
}