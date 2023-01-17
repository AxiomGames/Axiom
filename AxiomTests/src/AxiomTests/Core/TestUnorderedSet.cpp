

#include "doctest.h"
#include "Axiom/Core/UnorderedSet.hpp"
#include "Axiom/Core/String.hpp"

template<typename T>
static void CompareSets(const UnorderedSet<T>& s, const UnorderedSet<T>& expected)
{
	CHECK(s.Size() == expected.Size());

	typedef typename UnorderedSet<T>::ConstIterator iterator;
	for (iterator it = expected.begin(), end = expected.end(); it != end; ++it)
	{
		CHECK(s.Find(*it) != s.end());
	}
}


TEST_CASE("UnorderedSet_Constructor")
{
	using BaseSet = UnorderedSet<int>;

	BaseSet baseline{};
	baseline.Insert(5);
	baseline.Insert(6);
	CHECK(2 == baseline.Size());
	CHECK(baseline.Find(5) != baseline.end());
	CHECK(baseline.Find(6) != baseline.end());
	CompareSets(baseline, baseline);

	{
		BaseSet s;

		CHECK(s.Empty());
		CHECK(s.Size() == 0);
	}

	{
		BaseSet s = baseline;

		CompareSets(s, baseline);
	}

	{
		BaseSet other = baseline;
		BaseSet s = std::move(other);

		CompareSets(s, baseline);
		CHECK(other.Empty());
	}
}

TEST_CASE("UnorderedSet_Assign")
{
	using BaseSet = UnorderedSet<int>;

	BaseSet baseline;
	baseline.Insert(5);
	baseline.Insert(6);
	CHECK(2 == baseline.Size());
	CHECK(baseline.Find(5) != baseline.end());
	CHECK(baseline.Find(6) != baseline.end());
	CompareSets(baseline, baseline);

	{
		BaseSet s;
		s = baseline;

		CompareSets(s, baseline);
	}

	{
		BaseSet s;
		for (int ii = 0; ii != 10; ++ii)
			s.Insert(ii);

		s = baseline;

		CompareSets(s, baseline);
	}

	{
		BaseSet other = baseline;
		BaseSet s;
		s = std::move(other);

		CompareSets(s, baseline);
		CHECK(other.Empty());
	}

	{
		BaseSet other = baseline;
		BaseSet s;
		for (int ii = 0; ii != 10; ++ii)
		{
			s.Insert(ii);
		}

		s = std::move(other);

		CompareSets(s, baseline);
		CHECK(other.Empty());
	}
}

TEST_CASE("UnorderedSet_Insert")
{
	using BaseSet = UnorderedSet<String>;
	using BasePair = Pair<UnorderedSet<String>::Iterator, bool>;

	{
		BaseSet s;
		s.Insert("hello");
		CHECK(s.Find("hello") != s.end());
	}

	{
		BaseSet s;
		BasePair p1 = s.Insert("hello");
		CHECK(p1.second);
		CHECK((*p1.first) == String("hello"));

		BasePair p2 = s.Insert("hello");
		CHECK(!p2.second);
		CHECK(p2.first == p1.first);
	}

	{
		BaseSet s;
		s.Emplace("hello");

		CHECK(s.Find("hello") != s.end());
	}

	{
		BaseSet s;
		BasePair p1 = s.Emplace("hello");
		CHECK(p1.second);
		CHECK((*p1.first) == String("hello"));

		BasePair p2 = s.Emplace("hello");
		CHECK(!p2.second);
		CHECK(p2.first == p1.first);
	}

	{
		BaseSet s;
		String key("hello");
		s.Emplace(std::move(key));

		CHECK(s.Find("hello") != s.end());
		CHECK(key.Size() == 0);
	}
}

TEST_CASE("UnorderedSet_Copyctor")
{
	UnorderedSet<unsigned int> s{};
	s.Insert(32);
	UnorderedSet<unsigned int> other = s;
	CHECK(other.Find(32) != other.end());
	other.Clear();
	CHECK(other.Empty());
}
