#include "Axiom/Core/String.hpp"
#include "doctest.h"


TEST_CASE("TestString_Constructor")
{
	{
		String s;
		CHECK(s.Size() == 0);
	}
	{
		String s("hello");
		CHECK(s.Size() == 5);
		CHECK(0 == strcmp(s.CStr(), "hello"));
	}
	{
		String s("hello world", 5);
		CHECK(s.Size() == 5);
		CHECK(0 == strcmp(s.CStr(), "hello"));
	}
	{
		const String other("hello");
		String s = other;

		CHECK(s.Size() == 5);
		CHECK(0 == strcmp(s.CStr(), "hello"));
	}
	{
		String other("hello");
		String s = std::move(other);

		CHECK(s.Size() == 5);
		CHECK(0 == strcmp(s.CStr(), "hello"));
		CHECK(other.Size() == 0);
	}
}

TEST_CASE("TestString_Assign")
{
	{
		const String other("hello");
		String s("new");
		s = other;

		CHECK(s.Size() == 5);
		CHECK(0 == strcmp(s.CStr(), "hello"));
	}
	{
		String other("hello");
		String s("new");
		s = std::move(other);

		CHECK(s.Size() == 5);
		CHECK(0 == strcmp(s.CStr(), "hello"));
		CHECK(other.Size() == 0);
	}
}



