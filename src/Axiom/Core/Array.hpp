#pragma once

#include <cstdlib>
#include <cstdint>

namespace Ax
{
	template<typename T, int defaultSize = 12>
	class Array
	{
	private:
		static constexpr int CapacityBlockSize = 12;
	private:
		T* m_Data;
		uint32_t m_Size;
		uint32_t m_Capacity;
	public:
		Array() : m_Data(static_cast<T*>(malloc(sizeof(T) * defaultSize))), m_Size(0), m_Capacity(defaultSize) { }
		~Array() { if (m_Data != nullptr) free(m_Data); }

		Array(const Array& other) : m_Data(static_cast<T*>(malloc(sizeof(T) * other.m_Capacity))), m_Size(other.m_Size), m_Capacity(other.m_Capacity)
		{
			memcpy(m_Data, other.m_Data, other.m_Size * sizeof(T));
		}

		Array(Array&& other) noexcept : m_Data(other.m_Data), m_Size(other.m_Size), m_Capacity(other.m_Capacity)
		{
			other.m_Data = nullptr;
		}

		T* begin() { return m_Data; }
		T* end() { return m_Data + m_Size; }

		T* begin() const { return m_Data; }
		T* end() const { return m_Data + m_Size; }

		[[nodiscard]] bool Any() const { return m_Size > 0; }
		[[nodiscard]] bool Empty() const { return m_Size == 0; }

		T& operator[](int index)
		{
			return m_Data[index];
		}

		Array& Add(T type)
		{
			if (m_Size > m_Capacity)
			{
				m_Capacity += CapacityBlockSize;
				m_Data = static_cast<T*>(realloc(m_Data, sizeof(T) * m_Capacity));
			}

			m_Data[m_Size++] = type;

			return *this;
		}

		Array& Emplace(T&& type)
		{
			if (m_Size > m_Capacity)
			{
				m_Capacity += CapacityBlockSize;
				m_Data = static_cast<T*>(realloc(m_Data, sizeof(T) * m_Capacity));
			}

			m_Data[m_Size++] = std::move(type);

			return *this;
		}

		void Remove(T value)
		{
			for (uint32_t i = 0; i < m_Size; i++)
			{
				if (m_Data[i] == value)
				{
					memmove_s(m_Data + i, m_Size - i + 1, m_Data + i + 1, m_Size - i + 1);
					m_Size--;
					i--;
				}
			}
		}

		T& At(uint32_t index)
		{
			return m_Data[index];
		}

		[[nodiscard]] uint32_t Size() const { return m_Size; }
		[[nodiscard]] uint32_t DataSize() const { return m_Size * sizeof(T); }

		Array& operator+=(T type)
		{
			Add(type);
			return *this;
		}

		Array& operator-=(T type)
		{
			Remove(type);
			return *this;
		}
	};
}