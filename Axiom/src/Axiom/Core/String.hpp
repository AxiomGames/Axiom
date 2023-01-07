#pragma once

#include <cstdlib>
#include <sstream>
#include <iosfwd> // for overriding << operator for std::cout
#include "Common.hpp"
#include "Enum.hpp"

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

inline wchar_t* ToWCharArray(const char* string)
{
	const size_t len = strlen(string);
	wchar_t* buffer = (wchar_t*)malloc(len + 1ull * sizeof(wchar_t));
#pragma warning(suppress : 4996)
	mbstowcs(buffer, string, len + 1ull);
	return buffer;
}

inline char* ToCharArray(const wchar_t* string)
{
	const size_t len = wcslen(string);
	char* buffer = (char*)malloc(len + 1ull);
#pragma warning(suppress : 4996)
	wcstombs(buffer, string, len + 1ull);
	return buffer;
}

enum class StrResult : int
{
	None   = 0, NotFound = 0, False = 0, // negatives
	Found = 1, Success   = 1, True  = 1, // positives
	IndexOutOfArray = 2                   // errors
};

ENUM_FLAGS(StrResult, int);

class String
{
public:
	using CharType = char;

	~String() {
		if (m_Ptr) { free(m_Ptr); m_Ptr = nullptr;  m_Size = 0; m_Capacity = 0; }
	}
	String(int _size) : m_Size(0), m_Capacity(_size + 1)
	{
		m_Ptr = (CharType*)calloc(m_Capacity, sizeof(CharType));
	}
	// copy constructor
	String(const String& other) : m_Size(other.m_Size), m_Capacity(other.m_Capacity)
	{
		m_Ptr = (CharType*)calloc(m_Capacity, sizeof(CharType));
		memcpy(m_Ptr, other.m_Ptr, m_Size);
	}
	// move constructor 
	String(String&& other) noexcept : m_Size(other.m_Size), m_Capacity(other.m_Capacity), m_Ptr(other.m_Ptr)
	{
		other.m_Ptr = nullptr;
	}

	String() : m_Size(0), m_Capacity(32)
	{
		m_Ptr = (char*)calloc(m_Capacity, 1);
	}

	String(const char* _ptr) : m_Size(strlen(_ptr) + 1)
	{
		m_Capacity = m_Size + 32;
		m_Ptr = (char*)calloc(m_Capacity, 1);
		memcpy(m_Ptr, _ptr, m_Size);
	}

	String(CharType* _ptr) : m_Size(strlen(_ptr) + 1)
	{
		m_Capacity = m_Size + 32;
		m_Ptr = (CharType*)calloc(m_Capacity, 1);
		memcpy(m_Ptr, _ptr, m_Size);
	}
	// asign operator
	String& operator = (const String& right)
	{
		CapacityCheck(right.m_Size);
		memcpy(m_Ptr, right.m_Ptr, right.m_Size);
		m_Size = right.m_Size;
		return *this;
	}

	bool operator == (const String& b) const { return  Compare(*this, b); }
	bool operator != (const String& b) const { return !Compare(*this, b); }
	char operator[](int index)         const { return m_Ptr[index]; }

	CharType* begin() { return m_Ptr; }
	CharType* end()   { return m_Ptr + m_Size; }
	[[nodiscard]] const CharType* cbegin() const { return m_Ptr; }
	[[nodiscard]] const CharType* cend()   const { return m_Ptr + m_Size; }

	[[nodiscard]] const CharType* CStr() const { return m_Ptr; };

	static FINLINE bool Compare(const String& a, const String& b)
	{
		return a.m_Size == b.m_Size && !strcmp(a.m_Ptr, b.m_Ptr);
	}

	inline void Reset()
	{
		memset(m_Ptr, 0, m_Size);
		m_Size = 0;
	}

	inline void Set(const char* str)
	{
		Clear();
		CapacityCheck(strlen(str));
		memcpy(m_Ptr, str, strlen(str));
	}

	inline void Set(const String& str)
	{
		Clear();
		CapacityCheck(str.m_Size);
		m_Size = str.Length();
		memcpy(m_Ptr, str.CStr(), str.m_Size);
	}

	inline void Reserve(int size)
	{
		if (size > m_Capacity) {
			m_Capacity = size;
			m_Ptr = (CharType*)realloc(m_Ptr, m_Capacity);
		}
	}

	inline void Clear() { memset(m_Ptr, 0, m_Capacity); m_Size = 0; }

	// insert
	StrResult Insert(int index, char value)
	{
		if (index > m_Size || index < 0) return StrResult::IndexOutOfArray;
		const int oldLen = Length();
		Reserve(oldLen + 2);
		CharType* slow = m_Ptr + oldLen;
		CharType* fast = slow - 1;
		// shift all right characters to 1 char right
		while (fast >= m_Ptr + index)
		{
			*slow-- = *fast--;
		}
		m_Ptr[index] = value;
		m_Ptr[++m_Size] = '\0';
		return StrResult::Success;
	}
	
	StrResult Insert(int index, const char* other)
	{
		if (index > m_Size || index < 0) return StrResult::IndexOutOfArray;
		const int otherLen = (int)strlen(other);
		const int oldLen = Length();
		Reserve(oldLen + otherLen + 2);
		CharType* curr = m_Ptr + oldLen - 1;
		while (curr >= m_Ptr + index)
		{
			curr[otherLen + 1] = *curr--;
		}
		memcpy(m_Ptr + index, other, otherLen);
		m_Size += otherLen;
		m_Ptr[++m_Size] = '\0';
		return StrResult::Success;
	}

	StrResult Insert(int index, const String& value)
	{
		return Insert(index, value.CStr());
	}

	// time complexity O(numDigits(x)), space complexity O(1)
	void Append(int64 x, int afterPoint = 0)
	{
		int64 len = log10l(x);
		Reserve(m_Size + len + 1);
		int blen = len;
		while (++blen < afterPoint)
		{
			m_Ptr[m_Size++] = '0';
			m_Ptr[m_Size] = '\0';
		}
		len = powl(10, len);

		while (len)
		{
			int digit = x / len;
			m_Ptr[m_Size++] = char(digit + '0');
			m_Ptr[m_Size] = '\0';
			x -= len * digit;
			len /= 10;
		}
	}

	void Append(int x, int afterPoint = 0)
	{
		int len = log10(x);
		Reserve(m_Size + len + 1);
		
		// these 4 line is for adding zeros after float friction for example this two zeros: .005
		// another usage of this is using same number of digits for all numbers. example: 0053, 0145, 7984 but this is rare circumstance
		int blen = len;
		while (++blen < afterPoint)
		{
			m_Ptr[m_Size++] = '0';
			m_Ptr[m_Size] = '\0';
		}
		
		len = pow(10, len);

		while (len)
		{
			int digit = x / len;
			m_Ptr[m_Size++] = char(digit + '0');
			m_Ptr[m_Size] = '\0';
			x -= len * digit;
			len /= 10;
		}
	}

	/// <summary> appends as float </summary>
	/// <param name="afterpoint"> num digits after friction</param>
	void Append(float f, int afterpoint = 3)
	{
		int iPart = (int)f;
		Append(iPart);
		float fPart = f - iPart;
		AppendChar('.');
		Append(int(fPart * pow(10, afterpoint)), afterpoint);
	}

	static String From(int64 value) { String str{}; str.Append(value); return str; }
	static String From(int32 value) { String str{}; str.Append(value); return str; }
	static String From(float value) { String str{}; str.Append(value); return str; }

	void operator += (char _char)           { AppendChar(_char); }
	void operator += (const String& string) { Append(string);    }
	
	void operator += (int64 value) { Append(value); }
	void operator += (int32 value) { Append(value); }
	void operator += (float value) { Append(value); }

	String operator + (char _char)           const { String res(*this); res.AppendChar(_char);  return res; }
	String operator + (const char* string)   const { String res(*this); res.Append(string); return res; }
	String operator + (const String& string) const { String res(*this); res.Append(string); return res; }
	
	String operator + (int64 value) const { String res(*this); res.Append(value); return *this; }
	String operator + (int32 value) const { String res(*this); res.Append(value); return *this; }
	String operator + (float value) const { String res(*this); res.Append(value); return *this; }

	void AppendChar(char _char)
	{
		CapacityCheck(1);
		m_Ptr[m_Size++] = _char;
		m_Ptr[m_Size] = '\0';
	}

	inline void Append(const char* _char)
	{
		const size_t len = strlen(_char);
		CapacityCheck((int)len);
#pragma warning(suppress : 4996)
		strncat(m_Ptr + m_Size, _char, len + 1ull);
		m_Size += len;
		m_Ptr[m_Size] = '\0';
	}

	inline void Append(const char* _char, const size_t len)
	{
		CapacityCheck((int)len);
#pragma warning(suppress : 4996)
		strncat(m_Ptr + m_Size, _char, len + 1ull);
		m_Size += len;
		m_Ptr[m_Size] = '\0'; // for safety
	}

	inline String AppendCopy(const char* _char) const
	{
		const size_t len = strlen(_char);
		CharType* buffer = (CharType*)calloc(len + m_Size, sizeof(CharType));
		memcpy(buffer, m_Ptr, m_Size);
#pragma warning(suppress : 4996)
		strncat(buffer + m_Size, _char, len + 1ull);
		return String(buffer);
	}

	inline void Append(const String& string) { Append(string.CStr()); }

	// Find
	int FindIndex(char _char) const
	{
		for (int i = 0; i < m_Size; ++i)
			if (m_Ptr[i] == _char) return i;
		return -1;
	}

	inline int FindIndex(const char* str, const int len) const
	{
		for (int i = 0; i < m_Size - (len - 1); ++i)
			if (!strncmp(str, m_Ptr + i, len)) return i;
		return -1;
	}

	inline int FindIndex(const char* str) const
	{
		const int len = (int)strlen(str);
		for (int i = 0; i < m_Size - (len - 1); ++i)
			if (!strncmp(str, m_Ptr + i, len)) return i;
		return -1;
	}

	// Remove
	inline StrResult Remove(char _char)
	{
		const int index = FindIndex(_char);
		if (index == -1) return StrResult::NotFound;
		memmove(m_Ptr + index, m_Ptr + index + 1ull, 1ull);
		m_Ptr[--m_Size] = '\0';
		return StrResult::Success;
	}

	inline StrResult Remove(const char* _char)
	{
		const int otherLen  = strlen(_char);
		const int findIndex = FindIndex(_char, otherLen);

		if (findIndex != -1)
		{
			char* findPos  = m_Ptr + findIndex;
			char* otherEnd = findPos + otherLen;
			// shift all right characters to the find pos
			while (*otherEnd) {
				*findPos++ = *otherEnd++;
			}
			// set all removed characters to null terminator
			while (*findPos) {
				*findPos++ = '\0';
			}
			
			return StrResult::Success;
		}
		return StrResult::NotFound;
	}

	inline StrResult StartsWith(const char* other, int len) const
	{
		if (m_Size < len) StrResult::IndexOutOfArray;

		while (len--)
		{
			if (other[len] != m_Ptr[len]) return StrResult::False;
		}
		return StrResult::True;
	}

	StrResult StartsWith(const String& other) const
	{
		int len = other.Length();
		if (m_Size < len) StrResult::IndexOutOfArray;
		while (len--)
		{
			if (other[len] != m_Ptr[len]) return StrResult::False;
		}
		return StrResult::True;
	}

	StrResult Remove(const String& str)
	{
		return Remove(str.CStr());
	}

	StrResult Find(char _char) const        { return FindIndex(_char) ? StrResult::Success : StrResult::NotFound; }

	StrResult Find(const char* _char) const { return FindIndex(_char) ? StrResult::Success : StrResult::NotFound; }

	StrResult Find(const String& str) const { return FindIndex(str.CStr()) ? StrResult::Success : StrResult::NotFound; }

	StrResult Replace(int start, int end, const char* cstr)
	{
		if (end > m_Capacity) return StrResult::IndexOutOfArray;
		const int len = end - start;
		memcpy(m_Ptr + start, cstr, len);
		return StrResult::Success;
	}

	/// <summary> not suitable for big strings (1k-2k char) you can generate your own algorithm for that </summary>
	StrResult Replace(const char* from, const char* _new, int searchStartIndex = 0)
	{
		const int fromLen = (int)strlen(from);
		const int toLen   = (int)strlen(_new);

		int fromIndex = FindIndex(from, fromLen);

		if (fromIndex != -1) // this means finded
		{
			const int newLen = Length() + (fromLen - toLen);

			Reserve(newLen);
			m_Size = newLen;
			const int tailLen = strlen(m_Ptr + fromIndex + fromLen);
			memmove(m_Ptr + fromIndex + toLen, m_Ptr + fromIndex + fromLen, tailLen + 1 * sizeof(CharType));
			memcpy(m_Ptr + fromIndex, _new, toLen * sizeof(CharType));
			return StrResult::Success;
		}
		return StrResult::NotFound;
	}

	/// <summary> not suitable for big strings (1k-2k char) you can generate your own algorithm for that </summary>
	/// <returns> number of instance removed </returns>
	int ReplaceAll(const char* old, const char* _new)
	{
		StrResult strResult = StrResult::Success;
		const int _newLen = strlen(_new);
		int currentIndex = 0;
		int removedCount = 0;
		while (strResult == StrResult::Success) {
			strResult = Replace(old, _new, currentIndex);
			currentIndex += _newLen;
			++removedCount;
		}
		return removedCount;
	}
	
	StrResult Replace(const String& old, const String& _new)
	{
		return Replace(old.CStr(), _new.CStr());
	}

	String SubString(int begin, int end) const
	{
		if (begin > m_Size) return String();
		const int buffSize = end - begin;
		CharType* buffer = (CharType*)calloc(buffSize + 1ull, 1ull);
		memcpy(buffer, m_Ptr, buffSize);
		return String(buffer);
	}

	friend std::ostream& operator<<(std::ostream& cout, String& wstr) {
		return cout << wstr.CStr();
	}

	[[nodiscard]] int Capacity() const { return m_Capacity; }
	[[nodiscard]] int Length()   const { return m_Size; }
private:

	FINLINE void CapacityCheck(int len)
	{
		if (m_Size + len + 1 >= m_Capacity)
		{
			int newLen = m_Size + len;
			m_Capacity = newLen + (newLen / 2);
			m_Ptr = (CharType*)realloc(m_Ptr, m_Capacity + 1);
		}
	}
private:
	int m_Capacity;
	int m_Size;
	CharType* m_Ptr;
};

class WString
{
public:
	~WString()
	{
		if (ptr) { free(ptr); ptr = nullptr;  size = 0; capacity = 0; }
	}
	// copy constructor
	WString(const WString& other) : size(other.size), capacity(other.capacity)
	{
		ptr = (wchar_t*)calloc(capacity, sizeof(wchar_t));
		wmemcpy(ptr, other.ptr, size);
	}
	// move constructor 
	WString(WString&& other) noexcept : size(other.size), capacity(other.capacity), ptr(other.ptr)
	{
		other.ptr = nullptr;
	}

	WString() : size(0), capacity(32)
	{
		ptr = (wchar_t*)calloc(capacity, sizeof(wchar_t));
	}

	WString(int _size) : size(0), capacity(_size)
	{
		ptr = (wchar_t*)calloc(capacity, sizeof(wchar_t));
	}

	WString(const wchar_t* _ptr) : size(wcslen(_ptr))
	{
		capacity = size + 32;
		ptr = (wchar_t*)calloc(capacity, sizeof(wchar_t));
		wmemcpy(ptr, _ptr, size);
	}

	WString(wchar_t* _ptr) : size(wcslen(_ptr))
	{
		capacity = size + 32;
		ptr = (wchar_t*)calloc(capacity, sizeof(wchar_t));
		memcpy(ptr, _ptr, size * sizeof(wchar_t));
	}

	WString& operator = (const WString& right)
	{
		CapacityCheck(right.size);
		wmemcpy(ptr, right.ptr, right.size);
		size = right.size;
		return *this;
	}

	bool operator == (const WString& b) { return  Compare(*this, b); }
	bool operator != (const WString& b) { return !Compare(*this, b); }
	wchar_t operator[](int index) const { return ptr[index]; }

	wchar_t* begin() { return ptr; }
	wchar_t* end()   { return ptr + size; }
	const wchar_t* cbegin() const { return ptr; }
	const wchar_t* cend()   const { return ptr + size; }

	const wchar_t* CStr() const { return ptr; };

	static FINLINE bool Compare(WString a, WString b)
	{
		return a.size == b.size && !wcscmp(a.ptr, b.ptr);
	}
	
	inline void Reset()
	{
		memset(ptr, 0, size);
		size = 0;
	}

	inline void Set(const wchar_t* str)
	{
		Clear();
		CapacityCheck(wcslen(str));
		memcpy(ptr, str, wcslen(str) * sizeof(wchar_t));
	}

	inline void Set(const WString& str)
	{
		Clear();
		CapacityCheck(str.Length());
		size = str.Length();
		memcpy(ptr, str.CStr(), str.Length() * sizeof(wchar_t));
	}

	inline void Reserve(int size)
	{
		if (size > capacity) {
			capacity = size;
			ptr = (wchar_t*)realloc(ptr, capacity * sizeof(wchar_t));
		}
	}

	inline void Clear() { memset(ptr, 0, size * sizeof(wchar_t)); size = 0; }

	// Append
	inline StrResult Insert(int index, wchar_t value)
	{
		if (index > size || index < 0) return StrResult::IndexOutOfArray;
		const int oldLen = Length();
		Reserve(oldLen + 2);
		wchar_t* slow = ptr + oldLen;
		wchar_t* fast = slow - 1;
		// shift all right characters to 1 char right
		while (fast >= ptr + index)
		{
			*slow-- = *fast--;
		}
		ptr[index] = value;
		ptr[++size] = L'\0';
		return StrResult::Success;
	}

	inline StrResult Insert(int index, const wchar_t* other)
	{
		if (index > size || index < 0) return StrResult::IndexOutOfArray;
		const int otherLen = (int)wcslen(other);
		const int oldLen = Length();
		Reserve(oldLen + otherLen + 2);
		wchar_t* curr = ptr + oldLen - 1;
		while (curr >= ptr + index)
		{
			curr[otherLen + 1] = *curr--;
		}
		memcpy(ptr + index, other, otherLen * sizeof(wchar_t));
		size += otherLen;
		ptr[++size] = L'\0';
		return StrResult::Success;
	}

	inline StrResult Insert(int index, const WString& value)
	{
		return Insert(index, value.CStr());
	}

	void Append(int64 value) { wchar_t buff[16]; swprintf(buff, L"%lld", value); Append(buff); }
	void Append(int32 value) { wchar_t buff[16]; swprintf(buff, L"%d", value);   Append(buff); }
	void Append(float value) { wchar_t buff[16]; swprintf(buff, L"%f", value);   Append(buff); }

	static WString From(int64 value) { wchar_t buff[16]; swprintf(buff, L"%lld", value); return buff; }
	static WString From(int32 value) { wchar_t buff[16]; swprintf(buff, L"%d", value); return buff; }
	static WString From(float value) { wchar_t buff[16]; swprintf(buff, L"%f", value); return buff; }

	void operator += (char _char)            { AppendChar(_char); }
	void operator += (const WString& string) { Append(string);    }

	void operator += (int64 value) { Append(value); }
	void operator += (int32 value) { Append(value); }
	void operator += (float value) { Append(value); }

	WString operator + (wchar_t _char)         const { WString res = *this; res.Append(_char);  return res; }
	WString operator + (const WString& string) const { WString res = *this; res.Append(string); return res; }

	WString operator + (int64 value) const { WString res = *this; res.Append(value); return *this; }
	WString operator + (int32 value) const { WString res = *this; res.Append(value); return *this; }
	WString operator + (float value) const { WString res = *this; res.Append(value); return *this; }

	void AppendChar(wchar_t _char)
	{
		CapacityCheck(1);
		ptr[size++] = _char;
		ptr[size] = '\0'; 
	}

	WString Append(const wchar_t* _char)
	{
		const int len = wcslen(_char);
		CapacityCheck(len);
#pragma warning(suppress : 4996)
		wcsncat(ptr, _char, len + 1ull);
		size += len;
		ptr[size] = '\0'; 
		return *this;
	}

	WString Append(const wchar_t* _char, const size_t len)
	{
		CapacityCheck((int)len);
#pragma warning(suppress : 4996)
		wcsncat(ptr, _char, len + 1ull);
		size += len;
		ptr[size] = '\0'; 
		return *this;
	}

	WString AppendCopy(const wchar_t* _char) const
	{
		const size_t len = wcslen(_char);
		wchar_t* buffer = (wchar_t*)calloc(len + size, sizeof(wchar_t));
		wmemcpy(buffer, ptr, (size_t)size);
#pragma warning(suppress : 4996)
		wcsncat(buffer, _char, len + 1ull);
		return WString(buffer);
	}

	void Append(const WString& string) { Append(string.CStr()); }

	// Find
	inline int FindIndex(wchar_t _char) const
	{
		for (int i = 0; i < size; ++i)
			if (ptr[i] == _char) return i;
		return false;
	}

	inline int FindIndex(const wchar_t* str) const
	{
		const int len = wcslen(str);
		for (int i = 0; i < size - (len - 1); ++i)
			if (!wcsncmp(str, ptr + i, len)) return i;
		return -1;
	}

	inline int FindIndex(const wchar_t* str, const size_t wlen) const
	{
		const int len = (int)wlen;
		for (int i = 0; i < size - (len - 1); ++i)
			if (!wcsncmp(str, ptr + i, len)) return i;
		return -1;
	}
	// Remove
	inline StrResult Remove(wchar_t _char)
	{
		const int index = FindIndex(_char);
		if (index == -1) return StrResult::NotFound;
		memmove(ptr + index, ptr + index + 1, sizeof(wchar_t));
		ptr[--size] = L'\0';
		return StrResult::Success;
	}

	inline StrResult Remove(const wchar_t* _char)
	{
		const int otherLen  = wcslen(_char);
		const int findIndex = FindIndex(_char, otherLen);

		if (findIndex != -1)
		{
			wchar_t* findPos  = ptr + findIndex;
			wchar_t* otherEnd = findPos + otherLen;
			// shift all right characters to the find pos
			while (*otherEnd) {
				*findPos++ = *otherEnd++;
			}
			// set all removed characters to null terminator
			while (*findPos) {
				*findPos++ = L'\0';
			}
			
			return StrResult::Success;
		}
		return StrResult::NotFound;
	}

	inline StrResult StartsWith(const wchar_t* other, int len) const
	{
		if (size < len) StrResult::IndexOutOfArray;

		while (len--)
		{
			if (other[len] != ptr[len]) return StrResult::False;
		}
		return StrResult::True;
	}

	inline StrResult StartsWith(const WString& other) const
	{
		int len = other.Length();
		if (size < len) StrResult::IndexOutOfArray;

		while (len--)
		{
			if (other[len] != ptr[len]) return StrResult::False;
		}
		return StrResult::True;
	}

	StrResult Remove(const WString& str)
	{
		return Remove(str.CStr());
	}

	StrResult Find(wchar_t _char) const        { return FindIndex(_char) ? StrResult::Success : StrResult::NotFound; }

	StrResult Find(const wchar_t* _char) const { return FindIndex(_char) ? StrResult::Success : StrResult::NotFound; }

	StrResult Find(const WString& str) const   { return FindIndex(str.CStr()) ? StrResult::Success : StrResult::NotFound; }

	StrResult Replace(int start, int end, const wchar_t* cstr)
	{
		if (end > capacity) return StrResult::IndexOutOfArray;
		const int len = end - start;
		wmemcpy(ptr + start, cstr, len);
		return StrResult::Success;
	}

	StrResult Replace(const wchar_t* from, const wchar_t* _new, int searchStartIndex = 0)
	{
		const int fromLen = (int)wcslen(from);
		const int toLen   = (int)wcslen(_new);

		int fromIndex = FindIndex(from, fromLen);

		if (fromIndex != -1) // this means finded
		{
			const int newLen = Length() + (fromLen - toLen);
			Reserve(newLen);
			size = newLen;

			const size_t tailLen = wcslen(ptr + fromIndex + fromLen);
			memmove(ptr + fromIndex + toLen, ptr + fromIndex + fromLen, tailLen + 1ull * sizeof(char));
			memcpy(ptr + fromIndex, _new, toLen * sizeof(char));
			return StrResult::Success;
		}
		return StrResult::NotFound;
	}
	/// <summary> not suitable for big strings (1k-2k char) you can generate your own algorithm for that </summary>
	/// <returns> removed instance count </returns>
	int ReplaceAll(const wchar_t* old, const wchar_t* _new)
	{
		const int _newSize = wcslen(_new);
		int currentIndex = 0;
		int removedCount = 0;
		StrResult strResult = StrResult::Success;

		while (strResult == StrResult::Success) {
			strResult = Replace(old, _new, currentIndex);
			currentIndex += _newSize;
			++removedCount;
		}
		return removedCount;
	}

	StrResult Replace(const WString& old, const WString& _new)
	{
		return Replace(old.CStr(), _new.CStr());
	}

	String ToString() const
	{
		char* characters = (char*)malloc(size + 1 );
#pragma warning(suppress : 4996)
		wcstombs(characters, ptr, size + 1);
		return String(characters);
	}

	WString SubString(int begin, int end) const
	{
		if (begin > size) return WString();
		const int buffLen = end - begin;
		wchar_t* buffer = (wchar_t*)calloc(buffLen + 1ull, sizeof(wchar_t));
		wmemcpy(buffer, ptr, (size_t)buffLen);
		return WString(buffer);
	}

	friend std::ostream& operator << (std::ostream& cout, const WString& wstr) {
		return cout << ToCharArray(wstr.CStr());
	}

	int Capacity() const { return capacity; }
	int Length()   const { return size;     }

private:
	FINLINE void CapacityCheck(int len)
	{
		if (size + len + 1 >= capacity) 
		{
			int newLen = size + len;
			capacity = newLen + (newLen / 2);
			ptr = (wchar_t*)realloc(ptr, (size_t)capacity * sizeof(wchar_t));
		}
	}

private:
	int capacity;
	int size;
public:
	wchar_t* ptr = nullptr;
};

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
		return String::From(int64_t(((float)bytes / (float)megaBytes)))  + " MiB";
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
	wchar_t* buffer = (wchar_t*)malloc(string.Length() + 1ull * sizeof(wchar_t));
	#pragma warning(suppress : 4996)
	mbstowcs(buffer, string.CStr(), string.Length() + 1ull);
	return WString(buffer);
}

inline String ToString(const WString& string)
{
	char* buffer = (char*)malloc(string.Length() + 1ull * sizeof(char));
	#pragma warning(suppress : 4996)
	wcstombs(buffer, string.CStr(), string.Length() + 1ull);
	return String(buffer);
}

// inline String::Bytes operator<<(String& strOriginal, const String::Bytes& bytes)
// {
// 	return { &strOriginal };
// }

// inline String& operator<<(const String::Bytes& bytes, uint64_t num)
// {
// 	*bytes.Str += StringFormatBytes(num);
// 	return *bytes.Str;
// }
