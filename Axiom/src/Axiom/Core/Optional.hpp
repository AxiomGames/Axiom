#pragma once

#include "RawByteStorage.hpp"
#include "TemplateBase.hpp"

template<typename OptionalType>
class Optional
{
private:
	RawByteStorage<OptionalType> m_Value;
	bool m_IsSet;
public:
	Optional(const OptionalType& value)
	{
		new(&m_Value) OptionalType(value);
		m_IsSet = true;
	}

	Optional(OptionalType&& value)
	{
		new(&m_Value) OptionalType(Move(value));
		m_IsSet = true;
	}

	Optional() : m_IsSet(false) {}

	~Optional()
	{
		Reset();
	}

	Optional(const Optional& value) : m_IsSet(false)
	{
		if (value.m_IsSet)
		{
			new(&m_Value) OptionalType(*(const OptionalType*)&value.m_Value);
			m_IsSet = true;
		}
	}

	Optional(const Optional&& value) noexcept : m_IsSet(false)
	{
		if (value.m_IsSet)
		{
			new(&m_Value) OptionalType(Move(*(OptionalType*)&value.m_Value));
			m_IsSet = true;
		}
	}

	Optional& operator=(const Optional& value)
	{
		if (&value != this)
		{
			Reset();
			if (value.m_IsSet)
			{
				new(&m_Value) OptionalType(*(const OptionalType*)&value.m_Value);
				m_IsSet = true;
			}
		}

		return *this;
	}

	Optional& operator=(Optional&& value) noexcept
	{
		if (&value != this)
		{
			Reset();
			if (value.m_IsSet)
			{
				new(&m_Value) OptionalType(Move(*(OptionalType*)&value.m_Value));
				m_IsSet = true;
			}
		}

		return *this;
	}

	Optional& operator=(const OptionalType& value)
	{
		if (&value != (OptionalType*)&m_Value)
		{
			Reset();
			new(&m_Value) OptionalType(value);
			m_IsSet = true;
		}

		return *this;
	}

	Optional& operator=(OptionalType&& value)
	{
		if (&value != (OptionalType*)&m_Value)
		{
			Reset();
			new(&m_Value) OptionalType(Move(value));
			m_IsSet = true;
		}

		return *this;
	}

	void Reset()
	{
		if (m_IsSet)
		{
			m_IsSet = false;
			typedef OptionalType OptionalTypeForDestruct;
			((OptionalType*)&m_Value)->OptionalTypeForDestruct::~OptionalTypeForDestruct();
		}
	}

	template<typename... Args>
	OptionalType& Emplace(Args&&... args)
	{
		Reset();
		OptionalType* result = new(&m_Value) OptionalType(Forward<Args>(args)...);
		m_IsSet = true;
		return *result;
	}

	friend bool operator==(const Optional& lhs, const Optional& rhs)
	{
		if (lhs.m_IsSet != rhs.m_IsSet)
		{
			return false;
		}
		if (!lhs.m_IsSet) // both unset
		{
			return true;
		}
		return (*(OptionalType*)&lhs.m_Value) == (*(OptionalType*)&rhs.m_Value);
	}

	friend bool operator!=(const Optional& lhs, const Optional& rhs)
	{
		return !(lhs == rhs);
	}

	[[nodiscard]] bool IsSet() const { return m_IsSet; }
	FINLINE explicit operator bool() const { return m_IsSet; }

	const OptionalType& GetValue() const { return *(OptionalType*)&m_Value; }
	OptionalType& GetValue() { return *(OptionalType*)&m_Value; }

	const OptionalType* operator->() const { return &GetValue(); }
	OptionalType* operator->() { return &GetValue(); }

	const OptionalType& operator*() const { return GetValue(); }
	OptionalType& operator*() { return GetValue(); }
};
