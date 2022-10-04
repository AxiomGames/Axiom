#pragma once

#include "Archive.hpp"

class MemoryArchive : public Archive
{
private:
	using SizeType = uint64;

	uint8* m_Data = nullptr;
	size_t m_Cursor = 0;
	size_t m_Size = 10;
public:
	MemoryArchive()
	{
		m_Data = new uint8[m_Size];
	}

	MemoryArchive(uint8* data, SizeType size) : m_Data(data), m_Cursor(0), m_Size(size)
	{

	}

	~MemoryArchive()
	{
		delete[] m_Data;
	}

	FINLINE uint8* GetData() { return m_Data; }
	[[nodiscard]] FINLINE const uint8* GetData() const { return m_Data; }
	[[nodiscard]] FINLINE SizeType GetSize() const { return m_Size; }

	void ResetCursor()
	{
		m_Cursor = 0;
	}

	void Serialize(void* data, int64 length) override
	{
		if (IsLoading())
		{
			ax_assert(length >= m_Size);
			std::memcpy(data, m_Data + m_Cursor, length);
		}
		else
		{
			if (m_Cursor + length > m_Size)
			{
				m_Size = CalculateGrowth(m_Cursor + length);
				m_Data = static_cast<uint8*>(realloc(m_Data, m_Size));
			}

			std::memcpy(m_Data + m_Cursor, data, length);
		}

		m_Cursor += length;
	}

private:
	// Same as std::vector
	[[nodiscard]] FINLINE SizeType CalculateGrowth(SizeType newSize) const
	{
		const SizeType old_capacity = m_Size;
		const auto max = std::numeric_limits<SizeType>::max();

		if (old_capacity > max - old_capacity / 2) {
			return max; // geometric growth would overflow
		}

		const SizeType geometric_size = old_capacity + old_capacity / 2;

		if (geometric_size < newSize) {
			return newSize; // geometric growth would be insufficient
		}

		return geometric_size; // geometric growth is sufficient
	}
};
