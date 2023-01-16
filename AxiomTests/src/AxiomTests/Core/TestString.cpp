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

TEST_CASE("TestString_Empty")
{
	String s;
	CHECK(s.Empty());
	CHECK(s.Capacity() == 15);
	CHECK(s.begin() == s.end());
	CHECK(strlen(s.CStr()) == 0);
	CHECK(s == "");
}

TEST_CASE("TestString_Small")
{

	String s1("");

	CHECK(s1.Empty());
	CHECK(s1.Capacity() == 15);
	CHECK(s1.begin() == s1.end());
	CHECK(strlen(s1.CStr()) == 0);
	CHECK(s1 == "");

	String s2("hello");
	CHECK(s2.Size() == 5);
	CHECK(s2.Capacity() == 15);
	CHECK(s2.begin() + 5 == s2.end());
	CHECK(strlen(s2.CStr()) == 5);
	CHECK(s2 == "hello");

	String s3("exactly 15 char");
	CHECK(s3.Size() == 15);
	CHECK(s3.Capacity() == 15);
	CHECK(s3.begin() + 15 == s3.end());
	CHECK(strlen(s3.CStr()) == 15);
	CHECK(s3 == "exactly 15 char");
}

TEST_CASE("TestString_Long")
{
	const char* origin = "very long string larger than large string limit";
	size_t len = strlen(origin);
	String s(origin);
	CHECK(s.Size() == len);
	CHECK(s.Capacity() == len);
	CHECK(s.begin() + len == s.end());
	CHECK(strlen(s.CStr()) == len);
	CHECK(s == origin);
}

TEST_CASE("TestString_Assign")
{
	String s;
	const char* originshort = "short";
	size_t lenshort = strlen(originshort);
	s = originshort;
	CHECK(s.Size() == lenshort);
	CHECK(s.Capacity() == 15);
	CHECK(s.begin() + lenshort == s.end());
	CHECK(strlen(s.CStr()) == lenshort);
	CHECK(s == originshort);

	const char* originlong = "long long long long long long long long long long long long";
	size_t lenlong = strlen(originlong);
	s = originlong;
	CHECK(s.Size() == lenlong);
	CHECK(s.Capacity() == lenlong);
	CHECK(s.begin() + lenlong == s.end());
	CHECK(strlen(s.CStr()) == lenlong);
	CHECK(s == originlong);

	s = originshort;
	CHECK(s.Size() == lenshort);
	CHECK(s.Capacity() == lenlong);
	CHECK(s.begin() + lenshort == s.end());
	CHECK(strlen(s.CStr()) == lenshort);
	CHECK(s == originshort);
}

TEST_CASE("TestString_Swap")
{
	String ss1("short");
	String ss2("another");
	String sl1("long string for testing purposes");
	String sl2("another long string for testing purposes");

	ss1.Swap(ss2);
	CHECK(ss1 == "another");
	CHECK(ss2 == "short");

	sl1.Swap(sl2);
	CHECK(sl1 == "another long string for testing purposes");
	CHECK(sl2 == "long string for testing purposes");

	ss1.Swap(sl2);
	CHECK(ss1 == "long string for testing purposes");
	CHECK(sl2 == "another");

	sl1.Swap(ss2);
	CHECK(sl1 == "short");
	CHECK(ss2 == "another long string for testing purposes");
}

TEST_CASE("TestString_Equal")
{
	String s("hello");
	CHECK(s == String("hello"));
	CHECK(s == "hello");
	CHECK("hello" == s);
	CHECK(s != String("hello world"));
	CHECK(s != "hello world");
	CHECK("hello world" != s);
}

TEST_CASE("TestString_ltgt")
{
	String s("hello");
	CHECK(!(s < "hello"));
	CHECK(s < "helloo");
	CHECK(s < "hello0");
	CHECK(s > "he1");
	CHECK(s > "hell");
	CHECK(s > "a");
	CHECK(s < "z");
	CHECK(s > "aaaaaaaa");
	CHECK(s < "zzzzzzzz");
	CHECK(s > "hella");
	CHECK(s < "hellz");
	CHECK(s < "hellz");
}

TEST_CASE("TestString_lege")
{
	String s("hello");
	CHECK(s <= "hello");
	CHECK(s >= "hello");
	CHECK(s <= "helloo");
	CHECK(s <= "hello0");
	CHECK(s >= "he1");
	CHECK(s >= "hell");
	CHECK(s >= "a");
	CHECK(s <= "z");
	CHECK(s >= "aaaaaaaa");
	CHECK(s <= "zzzzzzzz");
	CHECK(s >= "hella");
	CHECK(s <= "hellz");
	CHECK(s <= "hellz");
}

TEST_CASE("TestString_Append")
{
	String s;
	s += "hello";
	s += ' ';
	s += "world";
	CHECK(s == "hello world");
	s += " and this is a very long string";
	CHECK(s == "hello world and this is a very long string");
}

TEST_CASE("TestString_add")
{
	CHECK(String("hello") + String(" world") == "hello world");
	CHECK(String("hello") + " world" == "hello world");
	CHECK(String("hello") + " " + "world" == "hello world");
	CHECK("hello" + String(" ") + "world" == "hello world");
}

TEST_CASE("TestString_Insert")
{
	String s("world");
	s.Insert(s.end(), '!');
	CHECK(s == "world!");
	s.Insert(s.begin(), "hello");
	CHECK(s == "helloworld!");
	s.Insert(s.begin() + 5, " ");
	CHECK(s == "hello world!");
	s.Insert(s.end() - 1, ", prepend a huge string to check");
	CHECK(s == "hello world, prepend a huge string to check!");
}

TEST_CASE("TestString_Erase")
{
	String s("hello");
	s.Erase(s.begin(), s.end());
	CHECK(s.Empty());
	s = "hello";
	s.Erase(s.end() - 1, s.end());
	CHECK(s == "hell");
	s = "hello world and this is a very long string";
	s.Erase(s.begin(), s.begin() + 4);
	CHECK(s == "o world and this is a very long string");
	s.Erase(s.begin(), s.end());
	CHECK(s.Empty());
}

TEST_CASE("TestString_Reserve")
{
	String s;
	s.Reserve(0);
	CHECK(s.Capacity() == 15);
	s.Reserve(10);
	s = "short";
	CHECK(s.Capacity() == 15);
	CHECK(s == "short");
	s.Reserve(15);
	CHECK(s.Capacity() == 15);
	CHECK(s == "short");
	s.Reserve(100);
	CHECK(s.Capacity() == 100);
	CHECK(s == "short");
	s.Reserve(101);
	CHECK(s.Capacity() == 101);
	CHECK(s == "short");
}

TEST_CASE("TestString_Resize")
{
	String s;
	s.Resize(1, ' ');
	CHECK(s == " ");
	s.Resize(16, '+');
	CHECK(s == " +++++++++++++++");
	s.Clear();
	s.Resize(16, '@');
	CHECK(s == "@@@@@@@@@@@@@@@@");
	s.Resize(12, '-');
	CHECK(s == "@@@@@@@@@@@@");
}
