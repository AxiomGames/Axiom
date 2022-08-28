#pragma once

#include "Common.hpp"

AX_NAMESPACE

class String;

class Archive
{
private:
	bool m_IsLoading : 1 = 0;
	bool m_IsSaving : 1 = 0;
public:
	Archive() = default;

	[[nodiscard]] FINLINE bool IsLoading() const { return m_IsLoading; }
	[[nodiscard]] FINLINE bool IsSaving() const { return m_IsSaving; }
	FINLINE void SetLoading(bool isLoading) { m_IsLoading = isLoading; }
	FINLINE void SetSaving(bool isSaving) { m_IsSaving = isSaving; }

	virtual void Serialize(void* data, int64 length) { }

	friend Archive& operator<<(Archive& archive, int32& value)
	{
		archive.Serialize(&value, sizeof(int32));
		return archive;
	}

	friend Archive& operator<<(Archive& archive, uint32& value)
	{
		archive.Serialize(&value, sizeof(uint32));
		return archive;
	}

	friend Archive& operator<<(Archive& archive, String& str);
};

AX_END_NAMESPACE