#pragma once

#include <cstdlib>
#include <cstdint>
#include <sstream>

namespace Ax
{
typedef uint64_t StrHashID;

constexpr StrHashID HashDjb2(const char* str)
{
	unsigned long int hash = 5381;

	char c = *str;
	while (c != 0)
	{
		hash = ((hash << 5) + hash) + c;
		c = *(++str);
	}

	return hash;
}

StrHashID constexpr operator "" _HASH(const char* s, std::size_t)
{
	return HashDjb2(s);
}

class String
{
public:
	using CharType = char;
	using LenType = uint32_t;
private:
	static constexpr int CapacityBlockSize = 16;
	static constexpr int AliasSize = 16;
	CharType* m_Data = nullptr;
	uint32_t m_Capacity = 0;
	CharType m_Alias[AliasSize]{'\0'};
public:
	__forceinline String() = default;

	~String()
	{
		if (m_Data)
		{
			free(m_Data);
			m_Data = nullptr;
		}
	}

	String(const char* str) // NOLINT(google-explicit-constructor)
	{
		auto len = strlen(str);

		if (len + 1 <= AliasSize)
		{
			memcpy(m_Alias, str, sizeof(CharType) * (len + 1));
			return;
		}

		m_Alias[0] = '\0';

		auto size = sizeof(CharType) * (len + 1);
		m_Capacity = len + 1;
		m_Data = (CharType*) malloc(size);
		strcpy_s(m_Data, size, str);
	}

	String(const String& other)
	{
		auto len = other.Length();

		if (len + 1 <= AliasSize)
		{
			memcpy(m_Alias, other.m_Alias, sizeof(CharType) * (len + 1));
			return;
		}

		m_Alias[0] = '\0';

		if (len == 0)
		{
			len = other.m_Capacity - 1;
		}

		auto size = sizeof(CharType) * (len + 1);
		m_Data = (CharType*) malloc(size);
		m_Capacity = other.m_Capacity;
		strcpy_s(m_Data, size, other.m_Data);
	}

	__forceinline String(String&& other) noexcept: m_Data(other.m_Data), m_Capacity(other.m_Capacity)
	{
		memcpy(m_Alias, other.m_Alias, 16);
		other.m_Data = nullptr;
	}

	__forceinline static String From(int32_t num)
	{
		String str;
		str.Append(num);
		return str;
	}

	__forceinline static String From(int64_t num)
	{
		String str;
		str.Append(num);
		return str;
	}

	__forceinline static String From(float num, const char* format = "%f")
	{
		String str;
		str.Append(num, format);
		return str;
	}

	__forceinline static String From(double num, const char* format = "%f")
	{
		String str;
		str.Append(num, format);
		return str;
	}

	__forceinline static String FormatBytes(uint64_t bytes)
	{
		uint64_t marker = 1024; // Change to 1000 if required
		uint64_t decimal = 3; // Change as required
		uint64_t kiloBytes = marker; // One Kilobyte is 1024 bytes
		uint64_t megaBytes = marker * marker; // One MB is 1024 KB
		uint64_t gigaBytes = marker * marker * marker; // One GB is 1024 MB
		uint64_t teraBytes = marker * marker * marker * marker; // One TB is 1024 GB

		// return bytes if less than a KB
		if (bytes < kiloBytes)
		{
			//ss << bytes << " Bytes";
			return String::From(int64_t(bytes)) + " B";
		} // return KB if less than a MB
		else if (bytes < megaBytes)
		{
			return String::From(int64_t(((float) bytes / (float) kiloBytes))) + " KiB";
		}// return MB if less than a GB
		else if (bytes < gigaBytes)
		{
			return String::From(int64_t(((float) bytes / (float) megaBytes))) + " MiB";
		} // return GB if less than a TB
		else if (bytes < teraBytes)
		{
			return String::From(int64_t((float) bytes / (float) gigaBytes)) + " GiB";
		}
		else
		{
			return String::From(int64_t((float) bytes / (float) teraBytes)) + " TiB";
		}
	}

	static __forceinline LenType StrLength(const CharType* str)
	{
		LenType len = 0;

		while (str[len] != '\0')
		{
			len++;
		}

		return len;
	}

	[[nodiscard]] __forceinline LenType Length() const
	{
		if (m_Alias[0] != '\0')
		{
			return StrLength(m_Alias);
		}
		else if (m_Data != nullptr)
		{
			return StrLength(m_Data);
		}
		else
		{
			return 0;
		}
	}

	[[nodiscard]] __forceinline size_t Size() const
	{ return Length() * sizeof(CharType); }

	[[nodiscard]] __forceinline StrHashID Hash() const
	{
		if (m_Alias[0] == '\0')
		{
			return HashDjb2(m_Data);
		}
		else
		{
			return HashDjb2(m_Alias);
		}
	}

	String& Append(const CharType* str, LenType len)
	{
		auto currentLen = Length();
		auto remainingSize = m_Capacity - (currentLen + 1);

		if (currentLen + len + 1 <= AliasSize)
		{
			memcpy(m_Alias + currentLen, str, len);
			m_Alias[currentLen + len] = '\0';
			return *this;
		}

		if (m_Data == nullptr)
		{
			auto newSize = currentLen + len + 1 + CapacityBlockSize;
			m_Data = static_cast<CharType*>(malloc(sizeof(CharType) * newSize));
			m_Capacity = newSize;

			if (m_Alias[0] != '\0')
			{
				memcpy(m_Data, m_Alias, sizeof(CharType) * currentLen);
			}
		}
		else if (len > remainingSize)
		{
			auto newSize = currentLen + len + 1 + CapacityBlockSize;
			m_Data = static_cast<CharType*>(realloc(m_Data, sizeof(CharType) * newSize));
			m_Capacity = newSize;
		}

		memcpy(m_Data + currentLen, str, len);
		m_Data[currentLen + len] = '\0';
		m_Alias[0] = '\0';

		return *this;
	}

	__forceinline String& Append(CharType c)
	{
		return Append(&c, 1);
	}

	__forceinline String& Append(const String& other)
	{
		return Append(other.m_Data, other.Length());
	}

	__forceinline void Append(int32_t num)
	{
		CharType buf[16];
		std::sprintf(buf, "%d", num);
		Append(buf, strlen(buf));
	}

	__forceinline void Append(int64_t num)
	{
		CharType buf[16];
		std::sprintf(buf, "%lld", num);
		Append(buf, strlen(buf));
	}

	__forceinline void Append(float num, const char* format = "%f")
	{
		CharType buf[16];
		std::sprintf(buf, format, num);
		Append(buf, strlen(buf));
	}

	__forceinline void Append(double num, const char* format = "%f")
	{
		CharType buf[16];
		std::sprintf(buf, format, num);
		Append(buf, strlen(buf));
	}

	String& operator=(const String& other)
	{
		memcpy(m_Alias, other.m_Alias, 16);

		if (other.m_Data == nullptr)
		{
			return *this;
		}

		if (other.m_Capacity > m_Capacity)
		{
			m_Data = static_cast<CharType*>(realloc(m_Data, sizeof(CharType) * m_Capacity));
			m_Capacity = other.m_Capacity;
		}

		for (LenType i = 0; i < other.Length() + 1; ++i)
		{
			m_Data[i] = other.m_Data[i];
		}

		return *this;
	}

	__forceinline void Clear()
	{
		m_Data[0] = '\0';
		m_Alias[0] = '\0';
	}

	__forceinline void Reset()
	{
		m_Alias[0] = '\0';

		if (m_Data != nullptr)
		{
			free(m_Data);
			m_Data = nullptr;
		}
	}

	__forceinline bool operator==(const String& other) const
	{ return strcmp(CStr(), other.CStr()) == 0; }

	__forceinline bool operator!=(const String& other) const
	{ return strcmp(CStr(), other.CStr()) != 0; }

	__forceinline bool operator==(const char* other) const
	{ return strcmp(CStr(), other) == 0; }

	__forceinline bool operator!=(const char* other) const
	{ return strcmp(CStr(), other) != 0; }

	__forceinline CharType operator[](int index)
	{
		if (m_Alias[0] != '\0')
		{
			return m_Alias[index];
		}
		else
		{
			return m_Data[index];
		}
	}

	__forceinline String& operator+(const char* str)
	{
		Append(str, strlen(str));
		return *this;
	}

	__forceinline String& operator+=(const char* str)
	{
		Append(str, strlen(str));
		return *this;
	}

	__forceinline String& operator+=(const String& str)
	{
		if (str.m_Alias[0] != '\0')
		{
			Append(str.m_Alias, str.Length());
			return *this;
		}

		Append(str.m_Data, str.Length());
		return *this;
	}

	__forceinline String& operator+=(int num)
	{
		Append(num);
		return *this;
	}

	__forceinline String& operator+=(float num)
	{
		Append(num);
		return *this;
	}

	__forceinline String& operator+=(double num)
	{
		Append(num);
		return *this;
	}

	[[nodiscard]] __forceinline const char* CStr() const
	{
		if (m_Alias[0] != '\0')
		{
			return m_Alias;
		}

		return m_Data;
	}

	struct Bytes
	{
		String* Str;
	};
};

inline String::Bytes operator<<(String& strOriginal, const String::Bytes& bytes)
{
	return {&strOriginal};
}

inline String& operator<<(const String::Bytes& bytes, uint64_t num)
{
	*bytes.Str += String::FormatBytes(num);
	return *bytes.Str;
}

inline String& operator<<(String& original, const char* str)
{
	original += str;
	return original;
}

inline String& operator<<(String& original, const String& added)
{
	original += added;
	return original;
}

inline std::ostream& operator<<(std::ostream& stream, const String& str)
{
	stream << str.CStr();
	return stream;
}
}