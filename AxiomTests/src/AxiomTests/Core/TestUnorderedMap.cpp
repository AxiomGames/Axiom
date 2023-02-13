#include "Axiom/Core/UnorderedMap.hpp"
#include "Axiom/Core/String.hpp"
#include "doctest.h"

#include <utility>

template<typename K, typename V>
static void CompareMaps(const UnorderedMap<K, V>& m, const UnorderedMap<K, V>& expected)
{
	CHECK(m.Size() == expected.Size());

	typedef typename UnorderedMap<K, V>::ConstIterator iterator;
	for (iterator it = expected.begin(), end = expected.end(); it != end; ++it)
	{
		iterator found = m.Find((*it).first);
		CHECK(found != m.end());
		CHECK((*found).second == (*it).second);
	}
}

TEST_CASE("TestUnorderedMap_Constructor")
{
	using BaseMap = UnorderedMap<int, int>;
	BaseMap baseline{};

	baseline.Insert(MakePair(5, 1));
	baseline.Insert(MakePair(6, 2));


	CHECK(2 == baseline.Size());
	CHECK(baseline.Find(5) != baseline.end());
	CHECK(baseline[5] == 1);
	CHECK(baseline.Find(6) != baseline.end());
	CHECK(baseline[6] == 2);

	CompareMaps(baseline, baseline);

	{
		BaseMap m;

		CHECK(m.Empty());
		CHECK(m.Size() == 0);
	}

	{
		BaseMap m = baseline;
		CompareMaps(m, baseline);
	}

	{
		BaseMap other = baseline;
		BaseMap m = std::move(other);

		CompareMaps(m, baseline);
		CHECK(other.Empty());
	}
}

TEST_CASE("TestUnorderedMap_Empty")
{
	UnorderedMap<int, int> empty{};
	CHECK(empty.Empty());
	{
		//copy
		auto other = empty;
		CHECK(other.Empty());

		//move
		auto moved = std::move(other);
		CHECK(moved.Empty());
	}

	auto it = empty.Find(10);
	CHECK(it == empty.end());

	int calc = 0;

	for (auto& emptyIt: empty)
	{
		calc++;
		calc += emptyIt.second;
	}

	for (const auto& emptyIt: empty)
	{
		calc++;
		calc += emptyIt.second;
	}
	CHECK(calc == 0);
	empty.Clear();
}

TEST_CASE("TestUnorderedMap_Assign")
{
	using BaseMap = UnorderedMap<int, int>;

	BaseMap baseline{};

	baseline.Insert(MakePair(5, 1));
	baseline.Insert(MakePair(6, 2));
	CHECK(2 == baseline.Size());
	CHECK(baseline.Find(5) != baseline.end());
	CHECK(baseline[5] == 1);
	CHECK(baseline.Find(6) != baseline.end());
	CHECK(baseline[6] == 2);
	CompareMaps(baseline, baseline);

	{
		BaseMap m;
		m = baseline;

		CompareMaps(m, baseline);
	}

	{
		BaseMap m;
		for (int ii = 0; ii != 10; ++ii)
			m.Insert(MakePair(ii, 10 * ii));

		m = baseline;

		CompareMaps(m, baseline);
	}

	{
		BaseMap other = baseline;
		BaseMap m;
		m = std::move(other);

		CompareMaps(m, baseline);
		CHECK(other.Empty());
	}

	{
		BaseMap other = baseline;
		BaseMap m;
		for (int ii = 0; ii != 10; ++ii)
			m.Insert(MakePair(ii, 10 * ii));

		m = std::move(other);

		CompareMaps(m, baseline);
		CHECK(other.Empty());
	}
}


TEST_CASE("TestUnorderedMap_Insert")
{
	using BaseMap = UnorderedMap<String, String>;
	using InsPair = Pair<BaseMap::Iterator, bool>;

	{
		BaseMap m;
		m.Insert(MakePair(String("hello"), String("world")));
		CHECK(m.Find("hello") != m.end());
	}

	{
		const Pair<String, String> p("hello", "world");
		BaseMap m;
		InsPair p1 = m.Insert(p);
		CHECK(p1.second);
		CHECK((*p1.first).first == String("hello"));
		CHECK((*p1.first).second == String("world"));

		InsPair p2 = m.Insert(p);
		CHECK(!p2.second);
		CHECK(p2.first == p1.first);
	}

	{
		BaseMap m;
		m.Emplace(Pair<String, String>("hello", "world"));

		CHECK(m.Find("hello") != m.end());
	}

	{
		BaseMap m;
		InsPair p1 = m.Emplace(Pair<String, String>("hello", "world"));
		CHECK(p1.second);
		CHECK((*p1.first).first == String("hello"));
		CHECK((*p1.first).second == String("world"));

		InsPair p2 = m.Emplace(Pair<String, String>("hello", "world"));
		CHECK(!p2.second);
		CHECK(p2.first == p1.first);
	}

	{
		BaseMap m;
		Pair<String, String> p("hello", "world");
		m.Emplace(std::move(p));

		CHECK(m.Find("hello") != m.end());
		CHECK(p.first.Size() == 0);
		CHECK(p.second.Size() == 0);
	}
}
