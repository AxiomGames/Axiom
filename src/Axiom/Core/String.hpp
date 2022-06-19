#pragma once

#include <cstdlib>
#include <cstdint>
#include <sstream>

#include "Common.hpp"
#include "Logger.hpp"

AX_NAMESPACE

typedef uint64_t StrHashID;

template<typename T>
constexpr StrHashID HashDjb2(const T* str)
{
	unsigned long int hash = 5381;

	T c = *str;
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

template<typename CharType, typename LenType>
class StringBase
{
public:
	// CharType must be char or wchar_t
	static_assert(
		std::is_same<CharType, char>() || std::is_same<CharType, wchar_t>()
	);

private:
	static constexpr int CapacityBlockSize = 16;
	static constexpr int AliasSize = 16;
	static constexpr CharType NullTerminator = (CharType)0;
	CharType* m_Data = nullptr;
	LenType m_Capacity = 0;
	CharType m_Alias[AliasSize]{ NullTerminator };
public:
	FINLINE StringBase() = default;

	~StringBase()
	{
		if (m_Data)
		{
			free(m_Data);
			m_Data = nullptr;
		}
	}

	StringBase(const CharType* str) // NOLINT(google-explicit-constructor)
	{
		LenType len = StrLength(str);

		if (len + 1 <= AliasSize)
		{
			memcpy(m_Alias, str, sizeof(CharType) * (len + 1));
			return;
		}

		m_Alias[0] = NullTerminator;

		LenType size = sizeof(CharType) * (len + 1);
		m_Capacity = len + 1;
		m_Data = (CharType*)malloc(size);
		StrCopy(m_Data, size, str);
	}

	StringBase(const StringBase& other)
	{
		LenType len = other.Length();

		if (len + 1 <= AliasSize)
		{
			memcpy(m_Alias, other.m_Alias, sizeof(CharType) * (len + 1));
			return;
		}

		m_Alias[0] = NullTerminator;

		if (len == 0)
		{
			len = other.m_Capacity - 1;
		}

		const LenType size = sizeof(CharType) * (len + 1);
		m_Data = (CharType*)malloc(size);
		m_Capacity = other.m_Capacity;
		StrCopy(m_Data, size, other.m_Data);
	}

	FINLINE StringBase(StringBase&& other) noexcept : m_Data(other.m_Data), m_Capacity(other.m_Capacity)
	{
		memcpy(m_Alias, other.m_Alias, 16);
		other.m_Data = nullptr;
	}

	[[nodiscard]] FINLINE const CharType* CStr() const
	{
		if (m_Alias[0] != NullTerminator)
		{
			return m_Alias;
		}
		return m_Data;
	}

	[[nodiscard]] FINLINE LenType Length() const
	{
		if (m_Alias[0] != NullTerminator)
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

	[[nodiscard]] FINLINE size_t Size() const { return Length() * sizeof(CharType); }

	[[nodiscard]] FINLINE StrHashID Hash() const
	{
		if (m_Alias[0] == NullTerminator)
		{
			return HashDjb2(m_Data);
		}
		else
		{
			return HashDjb2(m_Alias);
		}
	}

	FINLINE void Reserve(LenType requestedLen)
	{
		const LenType len = Length();
		const int addedLen = int(requestedLen - len);
		
		if (m_Alias[0] != NullTerminator && requestedLen > AliasSize)
		{
			m_Data = (CharType*)malloc(requestedLen + 1 * sizeof(CharType));
			memcpy(m_Data, m_Alias, len);
			memset(m_Data + len, 0, addedLen * sizeof(CharType)); // set added characters to null terminator
			m_Alias[0] = NullTerminator;
		}
		else if (m_Data && requestedLen > m_Capacity)
		{
			m_Data = (CharType*)realloc(m_Data, requestedLen + 1 * sizeof(CharType));
			memset(m_Data + len, 0, addedLen * sizeof(CharType)); // set added characters to null terminator
			m_Capacity = requestedLen;
		}
	}

	FINLINE void Clear()
	{
		m_Data[0] = NullTerminator;
		m_Alias[0] = NullTerminator;
	}

	FINLINE void Reset()
	{
		m_Alias[0] = NullTerminator;

		if (m_Data != nullptr)
		{
			free(m_Data);
			m_Data = nullptr;
		}
	}

	inline StringBase Substring(const LenType start, const LenType size)
	{
		const LenType end = start + size;
		CharType* newStr = (CharType*)malloc(size + 1 * sizeof(CharType));
		newStr[size] = NullTerminator;
		
		if (m_Alias[0] != NullTerminator && end < AliasSize)
		{
			memcpy(newStr, m_Alias + start, size * sizeof(CharType));
			return StringBase(newStr);
		}
		else if (m_Data && end < StrLength(m_Data))
		{
			memcpy(newStr, m_Data + start, size * sizeof(CharType));
			return StringBase(newStr);
		}
		return StringBase("Out of range!");
	}

	// returns if finded return index othervise return -1
	inline int Find(const CharType* other, int otherLen)
	{
		const CharType* str = CStr();
		int currIndex = 0;

		while (str[currIndex] != NullTerminator)
		{
			for (int i = 0; i < otherLen; ++i)
			{
				if (str[currIndex + i] == NullTerminator) return -1;
				if (str[currIndex + i] != other[i])      goto next_iter;
			}
			return currIndex;
		next_iter:
			currIndex++;
		}
		return -1;
	}

	FINLINE int Find(const CharType* str)
	{
		const int otherLen = (int)StrLength(str);
		return Find(str, otherLen);
	}

	// returns if finded return index othervise return -1
	inline int Find(const StringBase& str)
	{
		return Find(str.CStr());
	}

	// returns true if replaced correctly
	inline bool Replace(const CharType* from, const CharType* to, int fromLen, int toLen)
	{
		int fromIndex = Find(from, fromLen);

		if (fromIndex != -1) // this means finded
		{
			const int newLen = Length() + (fromLen - toLen);
			if (m_Alias[0] != NullTerminator && newLen > AliasSize)
			{
				m_Data = (CharType*)malloc(newLen + 1 * sizeof(CharType));
				memcpy(m_Data, m_Alias, newLen * sizeof(CharType));
				m_Data[newLen] = NullTerminator; // null terminator
			}

			Reserve(newLen);
			CharType* data = m_Alias[0] != NullTerminator ? m_Alias : m_Data;
			
			const int diff = abs(fromLen - toLen);
			const int tailLen = StrLength(data + fromIndex + fromLen);
			memmove(data + fromIndex + toLen, data + fromIndex + fromLen, tailLen + 1 * sizeof(CharType));
			memcpy(data + fromIndex, to, toLen * sizeof(CharType));
			return true;
		}
		return false;
	}

	inline bool ReplaceAll(const StringBase& from, const StringBase& to)
	{
		const int fromLen = (int)from.Length();
		const int toLen = (int)from.Length();
		while (Replace(from.CStr(), to.CStr(), fromLen, toLen));
	}

	// returns true if replaced correctly
	inline bool Replace(const StringBase& from, const StringBase& to)
	{
		return Replace(from.CStr(), to.CStr(), from.Length(), to.Length());
	}

	// returns true if removed correctly
	inline bool Remove(const CharType* str)
	{
		const int otherLen = StrLength(str);
		const int findIndex = Find(str, otherLen);

		if (findIndex != -1)
		{
			CharType* data = m_Alias[0] != NullTerminator ? m_Alias : m_Data;
			CharType* findPos  = data + findIndex;
			CharType* otherEnd = findPos + otherLen;
			// shift all right characters to the find pos
			while (*otherEnd) {
				*findPos++ = *otherEnd++;
			}
			// set all removed characters to null terminator
			while (*findPos) {
				*findPos++ = NullTerminator;
			}
			
			return true;
		}
		return false;
	}
	
	// returns true if removed correctly
	inline bool Remove(const StringBase& str)
	{
		return Remove(str.CStr());
	}

	inline void Insert(LenType index, const CharType value)
	{
		const int oldLen = (int)Length();
		Reserve(oldLen + 2);
		CharType* data = m_Alias[0] != NullTerminator ? m_Alias : m_Data;
		CharType* slow = data + oldLen;
		CharType* fast = slow - 1;
		// shift all right characters to 1 char right
		while (fast >= data + index)
		{
			*slow-- = *fast--;
		}
		data[index] = value;
	}

	inline void Insert(LenType index, const CharType* other)
	{
		const int otherLen = (int)StrLength(other);
		const int oldLen = Length();
		Reserve(oldLen + otherLen + 2);
		CharType* data = m_Alias[0] != NullTerminator ? m_Alias : m_Data;
		CharType* ptr = data + oldLen-1;
		while (ptr >= data + index)
		{
			ptr[otherLen+1] = *ptr--;
		}
		memcpy(data + index, other, otherLen * sizeof(CharType));
	}

	inline void Insert(LenType index, const StringBase& other)
	{
		Insert(index, other.CStr());
	}

	StringBase& Append(const CharType* str, LenType len)
	{
		LenType currentLen = Length();
		LenType remainingSize = m_Capacity - (currentLen + 1);

		if (currentLen + len + 1 <= AliasSize)
		{
			memcpy(m_Alias + currentLen, str, len);
			m_Alias[currentLen + len] = NullTerminator;
			return *this;
		}

		if (m_Data == nullptr)
		{
			LenType newSize = currentLen + len + 1 + CapacityBlockSize;
			m_Data = static_cast<CharType*>(malloc(sizeof(CharType) * newSize));
			m_Capacity = newSize;

			if (m_Alias[0] != NullTerminator)
			{
				memcpy(m_Data, m_Alias, sizeof(CharType) * currentLen);
			}
		}
		else if (len > remainingSize)
		{
			LenType newSize = currentLen + len + 1 + CapacityBlockSize;
			m_Data = static_cast<CharType*>(realloc(m_Data, sizeof(CharType) * newSize));
			m_Capacity = newSize;
		}

		memcpy(m_Data + currentLen, str, len * sizeof(CharType));
		m_Data[currentLen + len] = NullTerminator;
		m_Alias[0] = NullTerminator;

		return *this;
	}

	inline StringBase& Append(CharType c)
	{
		return Append(&c, 1);
	}

	inline StringBase& Append(const StringBase& other)
	{
		return Append(other.m_Data == nullptr ? other.m_Alias : other.m_Data, other.Length());
	}

	inline void Append(int32_t num)
	{
		CharType buf[16];
		if constexpr (std::is_same<CharType, char>())
			std::sprintf(buf, "%d", num);
		else if constexpr (std::is_same<CharType, wchar_t>())
			std::swprintf(buf, L"%d", num);

		Append(buf, StrLength(buf));
	}

	inline void Append(int64_t num)
	{
		CharType buf[16];
		if constexpr (std::is_same<CharType, char>())
			std::sprintf(buf, "%lld", num);
		else if constexpr (std::is_same<CharType, wchar_t>())
			std::swprintf(buf, L"%lld", num);
		Append(buf, StrLength(buf));
	}

	inline void Append(float num, const CharType* format = FloatingPointDefaultFormat())
	{
		CharType buf[16];
		Sprintf(buf, format, num);
		Append(buf, StrLength(buf));
	}

	inline void Append(double num, const CharType* format = FloatingPointDefaultFormat())
	{
		CharType buf[16];
		Sprintf(buf, format, num);
		Append(buf, StrLength(buf));
	}

	StringBase& operator = (const StringBase& other)
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

	FINLINE CharType operator[](int index)
	{
		return m_Alias[0] ? m_Alias[index] : m_Data[index];
	}

	inline StringBase& operator << (const CharType* str) { Append(str, StrLength(str)); return *this; }
	inline StringBase& operator << (const StringBase& added) { Append(added); return *this; }

	FINLINE bool operator == (const StringBase& other) const { return  StringCompare(CStr(), other.CStr()); }
	FINLINE bool operator != (const StringBase& other) const { return !StringCompare(CStr(), other.CStr()); }
	FINLINE bool operator == (const CharType* other) const { return  StringCompare(CStr(), other); }
	FINLINE bool operator != (const CharType* other) const { return !StringCompare(CStr(), other); }

	inline StringBase& operator +  (const char* str) { Append(str, StrLength(str)); return *this; }
	inline StringBase& operator += (const char* str) { Append(str, StrLength(str)); return *this; }

	inline StringBase& operator += (const StringBase& str)
	{
		if (str.m_Alias[0] != NullTerminator)
		{
			Append(str.m_Alias, str.Length());
			return *this;
		}

		Append(str.m_Data, str.Length());
		return *this;
	}

	inline StringBase& operator += (int num)    { Append(num); return *this; }
	inline StringBase& operator += (float num)  { Append(num); return *this; }
	inline StringBase& operator += (double num) { Append(num); return *this; }

public:
	inline static StringBase From(int32_t num) { StringBase str; str.Append(num); return str; }

	inline static StringBase From(int64_t num) { StringBase str; str.Append(num); return str; }

	inline static StringBase FromFloat(float num, const CharType* format = FloatingPointDefaultFormat())
	{
		StringBase str;
		str.Append(num, format);
		return str;
	}

	inline static StringBase FromDouble(double num, const CharType* format = FloatingPointDefaultFormat())
	{
		StringBase str;
		str.Append(num, format);
		return str;
	}

	FINLINE static LenType StrLength(const CharType* str)
	{
		LenType len = 0;

		while (str[len] != NullTerminator) len++;

		return len;
	}

	FINLINE static void StrCopy(CharType* str, LenType len, const CharType* other)
	{
		while (*other != NullTerminator)
		{
			*str++ = *other++;
		}
		*str = NullTerminator;
	}

	// returns true if pointers are same
	FINLINE static bool StringCompare(const CharType* const a, const CharType* const b)
	{
		while (true)
		{
			if ((*a == NullTerminator || *b == NullTerminator) || *a != *b)
				return false;
			a++; b++;
		}
		return true;
	}

	template<typename T>
	static void Sprintf(CharType* const buff, const CharType* const format, T value)
	{
		if constexpr (std::is_same<CharType, char>())
		{
			std::sprintf(buff, format, value);
		}
		else if constexpr (std::is_same<CharType, wchar_t>())
		{
			std::swprintf(buff, format, value);
		}
	}
private:
	static constexpr const CharType* FloatingPointDefaultFormat()
	{
		if constexpr (std::is_same<CharType, char>())    return "%f";
		if constexpr (std::is_same<CharType, wchar_t>()) return L"%f";
	}
public:
	struct Bytes
	{
		StringBase* Str;
	};
};

using String  = StringBase<char, int>;
using WString = StringBase<wchar_t, int>;

inline bool BigStringCompareEqualsSSE(const String& a, const String& b)
{
	return false;
}

inline bool BigStringCompareEqualsSSE(const WString& a, const WString& b)
{
	return false;
}

inline String StringFormatBytes(uint64_t bytes)
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
		return String::From(int64_t(((float)bytes / (float)kiloBytes))) + " KiB";
	}// return MB if less than a GB
	else if (bytes < gigaBytes)
	{
		return String::From(int64_t(((float)bytes / (float)megaBytes))) + " MiB";
	} // return GB if less than a TB
	else if (bytes < teraBytes)
	{
		return String::From(int64_t((float)bytes / (float)gigaBytes)) + " GiB";
	}
	else
	{
		return String::From(int64_t((float)bytes / (float)teraBytes)) + " TiB";
	}
}

inline WString ToWString(const String& string)
{
	wchar_t* buffer = (wchar_t*)std::malloc(string.Size() + 1 * sizeof(wchar_t));
	#pragma warning(suppress : 4996)
	std::mbstowcs(buffer, string.CStr(), string.Length() + 1);
	return WString(buffer);
}

inline String ToString(const WString& string)
{
	char* buffer = (char*)std::malloc(string.Size() + 1 * sizeof(char));
	#pragma warning(suppress : 4996)
	std::wcstombs(buffer, string.CStr(), string.Length() + 1);
	return String(buffer);
}

inline wchar_t* ToWCharArray(const char* string)
{
	const int len = std::strlen(string);
	wchar_t* buffer = (wchar_t*)std::malloc(len + 1 * sizeof(wchar_t));
	#pragma warning(suppress : 4996)
	std::mbstowcs(buffer, string, len + 1);
	return buffer;
}

inline char* ToCharArray(const wchar_t* string)
{
	const int len = std::wcslen(string);
	char* buffer = (char*)std::malloc(len + 1);
	#pragma warning(suppress : 4996)
	std::wcstombs(buffer, string, len + 1);
	return buffer;
}

inline std::ostream& operator << (std::ostream& cout, String& str)  { return cout << str.CStr(); }
inline std::ostream& operator << (std::ostream& cout, WString& str) { return cout << ToString(str).CStr(); }

inline String::Bytes operator<<(String& strOriginal, const String::Bytes& bytes)
{
	return { &strOriginal };
}

inline String& operator<<(const String::Bytes& bytes, uint64_t num)
{
	*bytes.Str += StringFormatBytes(num);
	return *bytes.Str;
}


AX_END_NAMESPACE