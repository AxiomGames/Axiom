#include "doctest.h"

#include "Axiom/Core/Array.hpp"


TEST_CASE("Array_Constructor")
{
	using BaseArray = Array<int>;

	{
		BaseArray v;
		CHECK(v.Empty());
		CHECK(v.Count() == 0);
	}
	{
		const int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		BaseArray v(array, array + 10);

		CHECK(v.Count() == 10);
		CHECK(std::equal(v.begin(), v.end(), array));
	}
	{
		const int value = 127;
		const size_t count = 24;
		BaseArray v(count, value);

		CHECK(v.Count() == count);

		BaseArray::Iterator it = v.begin(), end = v.end();
		for (; it != end; ++it)
			CHECK(*it == value);
	}
	{
		const size_t count = 24;
		BaseArray v(count);

		CHECK(v.Count() == count);
		BaseArray::Iterator it = v.begin(), end = v.end();
		for (; it != end; ++it)
			CHECK(*it == 0);
	}
	{
		const int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		BaseArray other(array, array + 10);
		BaseArray v = other;

		CHECK(v.Count() == other.Count());
		CHECK(std::equal(v.begin(), v.end(), other.begin()));
	}

	{
		const int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		BaseArray other(array, array + 10);
		BaseArray v = std::move(other);

		CHECK(v.Count() == 10);
		CHECK(std::equal(v.begin(), v.end(), array));
		CHECK(other.Count() == 0);
	}
}

TEST_CASE("Array_Assignment")
{
	using BaseArray = Array<int>;
	{
		const int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		BaseArray other(array, array + 10);

		BaseArray v;
		v = other;

		CHECK(v.Count() == 10);
		CHECK(std::equal(v.begin(), v.end(), array));
		CHECK(other.Count() == 10);
		CHECK(std::equal(v.begin(), v.end(), other.begin()));
	}
}

TEST_CASE("Array_PushBack")
{
	Array<int> v;
	v.PushBack(42);

	CHECK(v.Count() == 1);
	CHECK(v[0] == 42);
}

TEST_CASE("Array_Array")
{
	Array<Array<int> > v(10, Array<int>());
	Array<Array<int> >::Iterator it = v.begin(), end = v.end();
	for (; it != end; ++it)
	{
		CHECK((*it).Empty());
		CHECK((*it).Count() == 0);
		CHECK((*it).begin() == (*it).end());
	}
}

TEST_CASE("Vector_Swap")
{
	Array<int> v1;
	v1.PushBack(12);
	v1.PushBack(20);

	Array<int> v2;
	v2.PushBack(54);

	v1.Swap(v2);

	CHECK(v1.Count() == 1);
	CHECK(v2.Count() == 2);
	CHECK(v1[0] == 54);
	CHECK(v2[0] == 12);
	CHECK(v2[1] == 20);
}

TEST_CASE("Array_PopBack")
{
	Array<int> v;
	v.PushBack(12);
	v.PushBack(24);

	CHECK(v.Back() == 24);

	v.PopBack();

	CHECK(v.Back() == 12);
	CHECK(v.Count() == 1);
}

TEST_CASE("vector_Assign") {
	Array<int> v;

	CHECK(v.Count() == 0);

	const int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	v.Assign(array, array + 10);
	CHECK(v.Count() == 10);
	CHECK( std::equal(v.begin(), v.end(), array) );
}

TEST_CASE("vector_erase") {
	const int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	Array<int> v(array, array + 10);

	Array<int>::Iterator it = v.Erase(v.begin());
	CHECK(*it == 2);
	CHECK(v.Count() == 9);
	CHECK( std::equal(v.begin(), v.end(), array + 1) );

	it = v.Erase(v.end() - 1);
	CHECK(it == v.end());
	CHECK(v.Count() == 8);
	CHECK( std::equal(v.begin(), v.end(), array + 1) );

	v.Erase(v.begin() + 1, v.end() - 1);
	CHECK(v.Count() == 2);
	CHECK(v[0] == 2);
	CHECK(v[1] == 9);
}

TEST_CASE("vector_erase_unordered") {
	const int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	typedef Array<int> vector;
	vector v(array, array + 10);

	int first = *(v.begin());
	vector::Iterator it = v.EraseUnordered(v.begin());
	CHECK(it == v.begin());
	CHECK(v.Count() == 9);
	CHECK( std::count(v.begin(), v.end(), first) == 0 );
	for (it = v.begin(); it != v.end(); ++it) {
		CHECK( std::count(v.begin(), v.end(), *it) == 1 );
	}

	int last = *(v.end() - 1);
	it = v.EraseUnordered(v.end() - 1);
	CHECK(it == v.end());
	CHECK(v.Count() == 8);
	CHECK( std::count(v.begin(), v.end(), last) == 0 );
	for (it = v.begin(); it != v.end(); ++it) {
		CHECK( std::count(v.begin(), v.end(), *it) == 1 );
	}

	first = *(v.begin());
	last = *(v.end() - 1);
	v.EraseUnordered(v.begin() + 1, v.end() - 1);
	CHECK(v.Count() == 2);
	CHECK( std::count(v.begin(), v.end(), first) == 1 );
	CHECK( std::count(v.begin(), v.end(), last) == 1 );
}

TEST_CASE("vector_insert") {
	const int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	Array<int> v(array, array + 10);

	v.Insert(v.begin(), 0);
	CHECK(v.Count() == 11);
	CHECK(v[0] == 0);
	CHECK( std::equal(v.begin() + 1, v.end(), array) );

	v.Insert(v.end(), 11);
	CHECK(v.Count() == 12);
	CHECK(v[0] == 0);
	CHECK( std::equal(array, array + 10, v.begin() + 1) );
	CHECK(v.Back() == 11);

	const int array2[3] = {11, 12, 13};
	const int finalarray[] = {0, 1, 2, 3, 11, 12, 13, 4, 5, 6, 7, 8, 9, 10, 11};
	v.Insert(v.begin() + 4, array2, array2 + 3);
	CHECK( v.Count() == 15 );
	CHECK( std::equal(v.begin(), v.end(), finalarray) );
}

TEST_CASE("vector_Iterator") {
	const int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	Array<int> v(array, array + 10);
	const Array<int>& cv = v;

	CHECK(v.Data() == &*v.begin());
	CHECK(v.Data() == &v[0]);
	CHECK(v.Data() + v.Count() == &*v.end());
	CHECK(v.begin() == cv.begin());
	CHECK(v.end() == cv.end());
	CHECK(v.Data() == cv.Data());

	Array<int> w = v;
	CHECK(v.begin() != w.begin());
	CHECK(v.end() != w.end());
}