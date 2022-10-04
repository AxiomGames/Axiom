#include "String.hpp"
#include "Archive.hpp"

Archive& operator<<(Archive& archive, String& str)
{
	int32 numChars = archive.IsLoading() ? 0 : str.Length();
	archive << numChars;

	if (archive.IsLoading())
	{
		String::CharType* chars = new String::CharType[numChars + 1];
		archive.Serialize(chars, (numChars + 1) * sizeof(String::CharType));

		str.Set(chars);
		delete[] chars;
	}
	else
	{
		archive.Serialize(str.begin(), (numChars + 1) * sizeof(String::CharType));
	}

	return archive;
}
