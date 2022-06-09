#pragma once

#include <cstdint>

namespace Ax
{
	class String
	{
	public:
		using CharType = char;
		using LenType = uint32_t;
	private:
		static constexpr int CapacityBlockSize = 10;
		CharType* m_Data = nullptr;
		uint32_t m_Capacity = 0;
	public:
		String()
		{
			m_Capacity = CapacityBlockSize;
			m_Data = (CharType*)malloc(sizeof(CharType) * m_Capacity);
			m_Data[0] = '\0';
		}

		~String()
		{
			free(m_Data);
			m_Data = nullptr;
		}

		String(const char* str)
		{
			auto len = strlen(str);
			auto size = sizeof(CharType) * (len + 1);
			m_Capacity = len + 1;
			m_Data = (CharType*)malloc(size);
			strcpy_s(m_Data, size, str);
		}

		String(const String& other)
		{
			auto len = strlen(other.m_Data);
			auto size = sizeof(CharType) * (len + 1);
			m_Data = (CharType*)malloc(size);
			m_Capacity = other.m_Capacity;
			strcpy_s(m_Data, size, other.m_Data);
		}

		String(const String&& other) noexcept : m_Data(other.m_Data), m_Capacity(other.m_Capacity) {}

		static String FromInt(int32_t num)
		{
			String str;
			str.AppendInt(num);
			return str;
		}

		static LenType StrLength(CharType* str)
		{
			LenType len = 0;

			while(*str != '\0')
			{
				len++;
				str++;
			}

			return len;
		}

		[[nodiscard]] LenType Length() const { return StrLength(m_Data); }
		[[nodiscard]] size_t Size() const { return Length() * sizeof(CharType); }

		String& Append(const CharType* str, LenType len)
		{
			auto currentLen = Length();
			auto remainingSize = m_Capacity - (currentLen + 1);

			if (len > remainingSize)
			{
				auto newSize = currentLen + len + 1 + CapacityBlockSize;
				m_Data = static_cast<CharType*>(realloc(m_Data, sizeof(CharType) * newSize));
				m_Capacity = newSize;
			}

			for (LenType i = 0; i < len; ++i)
			{
				m_Data[currentLen + i] = str[i];
			}

			m_Data[currentLen + len] = '\0';

			return *this;
		}

		String& Append(CharType c)
		{
			return Append(&c, 1);
		}

		String& Append(const String& other)
		{
			return Append(other.m_Data, other.Length());
		}

		void AppendInt(int32_t num)
		{
			CharType buf[16];
			std::sprintf(buf, "%d", num);
			Append(buf, strlen(buf));
		}

		void AppendFloat(float num, const char* format = "%f")
		{
			CharType buf[16];
			std::sprintf(buf, format, num);
			Append(buf, strlen(buf));
		}

		void AppendDouble(double num, const char* format = "%f")
		{
			CharType buf[16];
			std::sprintf(buf, format, num);
			Append(buf, strlen(buf));
		}

		String& operator=(const String& other)
		{
			if (other.m_Capacity > m_Capacity)
			{
				m_Data = static_cast<CharType*>(realloc(m_Data, sizeof(CharType) * m_Capacity));
				m_Capacity = other.m_Capacity;
			}

			for (LenType i = 0; i < other.Length(); ++i)
			{
				m_Data[i] = other.m_Data[i];
			}

			return *this;
		}

		void Clear()
		{
			m_Data[0] = '\0';
		}

		void Reset()
		{
			if (m_Capacity > CapacityBlockSize)
			{
				m_Data = static_cast<CharType*>(realloc(m_Data, sizeof(CharType) * CapacityBlockSize));
			}

			m_Data[0] = '\0';
		}

		bool operator==(const String& other) { return strcmp(m_Data, other.m_Data) == 0; }
		bool operator!=(const String& other) { return strcmp(m_Data, other.m_Data) != 0; }

		bool operator==(const char* other) { return strcmp(m_Data, other) == 0; }
		bool operator!=(const char* other) { return strcmp(m_Data, other) != 0; }

		CharType operator[](int index) { return m_Data[index]; }

		[[nodiscard]] const char* CStr() const { return m_Data; }
	};
}