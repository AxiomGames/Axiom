
#include "Axiom/Core/Span.hpp"
#include "doctest.h"
#include "Axiom/Core/Array.hpp"


TEST_CASE("test-span")
{

	Array<int> vec{};
	vec.PushBack(1);
	vec.PushBack(3);
	vec.PushBack(5);

	{
		Span<int> span = vec;

		CHECK(!span.Empty());
		CHECK(span.Size() == 3);
		CHECK(span[0] == 1);
		CHECK(span[1] == 3);
		CHECK(span[2] == 5);
		CHECK(span.Back() == 5);

		int sum = 0;
		for (auto& vl: span)
		{
			sum += vl;
		}
		CHECK(sum == 9);
	}

	{
		const Span<int> constSpan = vec;
		CHECK(!constSpan.Empty());
		CHECK(constSpan.Size() == 3);
		CHECK(constSpan[0] == 1);
		CHECK(constSpan[1] == 3);
		CHECK(constSpan[2] == 5);
		CHECK(constSpan.Back() == 5);

		int sum = 0;
		for (auto& vl: constSpan)
		{
			sum += vl;
		}
		CHECK(sum == 9);
	}
}

TEST_CASE("test-empty-span")
{
	Span <int> span{};
	CHECK(span.Empty());
	CHECK(span.Size() == 0);
	CHECK(span.begin() == span.end());
}
