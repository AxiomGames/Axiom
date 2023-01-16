

#include "doctest.h"
#include "Axiom/Core/UnorderedSet.hpp"
#include "Axiom/Core/String.hpp"

template<typename T>
static void CompareSets(const UnorderedSet<T>& s, const UnorderedSet<T>& expected)
{
	CHECK(s.size() == expected.size());

	typedef typename UnorderedSet<T>::const_iterator iterator;
	for (iterator it = expected.begin(), end = expected.end(); it != end; ++it)
	{
		CHECK(s.find(*it) != s.end());
	}
}


TEST_CASE("UnorderedSet_Constructor")
{
	using BaseSet = UnorderedSet<int>;

	BaseSet baseline{};
	baseline.insert(5);
	baseline.insert(6);
	CHECK(2 == baseline.size());
	CHECK(baseline.find(5) != baseline.end());
	CHECK(baseline.find(6) != baseline.end());
	CompareSets(baseline, baseline);

	{
		BaseSet s;

		CHECK(s.empty());
		CHECK(s.size() == 0);
	}

	{
		BaseSet s = baseline;

		CompareSets(s, baseline);
	}

	{
		BaseSet other = baseline;
		BaseSet s = std::move(other);

		CompareSets(s, baseline);
		CHECK(other.empty());
	}
}

TEST_CASE("UnorderedSet_Assign")
{
	using BaseSet = UnorderedSet<int>;

	BaseSet baseline;
	baseline.insert(5);
	baseline.insert(6);
	CHECK(2 == baseline.size());
	CHECK(baseline.find(5) != baseline.end());
	CHECK(baseline.find(6) != baseline.end());
	CompareSets(baseline, baseline);

	{
		BaseSet s;
		s = baseline;

		CompareSets(s, baseline);
	}

	{
		BaseSet s;
		for (int ii = 0; ii != 10; ++ii)
			s.insert(ii);

		s = baseline;

		CompareSets(s, baseline);
	}

	{
		BaseSet other = baseline;
		BaseSet s;
		s = std::move(other);

		CompareSets(s, baseline);
		CHECK(other.empty());
	}

	{
		BaseSet other = baseline;
		BaseSet s;
		for (int ii = 0; ii != 10; ++ii)
		{
			s.insert(ii);
		}

		s = std::move(other);

		CompareSets(s, baseline);
		CHECK(other.empty());
	}
}

TEST_CASE("UnorderedSet_Insert")
{
	using BaseSet = UnorderedSet<String>;
	using BasePair = Pair<UnorderedSet<String>::iterator, bool>;

	{
		BaseSet s;
		s.insert("hello");
		CHECK(s.find("hello") != s.end());
	}

	{
		BaseSet s;
		BasePair p1 = s.insert("hello");
		CHECK(p1.second);
		CHECK((*p1.first) == String("hello"));

		BasePair p2 = s.insert("hello");
		CHECK(!p2.second);
		CHECK(p2.first == p1.first);
	}

	{
		BaseSet s;
		s.emplace("hello");

		CHECK(s.find("hello") != s.end());
	}

	{
		BaseSet s;
		BasePair p1 = s.emplace("hello");
		CHECK(p1.second);
		CHECK((*p1.first) == String("hello"));

		BasePair p2 = s.emplace("hello");
		CHECK(!p2.second);
		CHECK(p2.first == p1.first);
	}

	{
		BaseSet s;
		String key("hello");
		s.emplace(std::move(key));

		CHECK(s.find("hello") != s.end());
		CHECK(key.Size() == 0);
	}
}

TEST_CASE("UnorderedSet_Copyctor")
{
	UnorderedSet<unsigned int> s{};
	s.insert(32);
	UnorderedSet<unsigned int> other = s;
	CHECK(other.find(32) != other.end());
	other.clear();
	CHECK(other.empty());
}
