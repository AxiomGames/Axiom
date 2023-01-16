#include "doctest.h"

#include "Axiom/Core/Array.hpp"


TEST_CASE("Array_Constructor") {
	using BaseArray = Array<int>;

	{
		BaseArray v;
		CHECK( v.Empty() );
		CHECK(v.Count() == 0);
	}
	{
		const int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
		BaseArray v(array, array + 10);

		CHECK( v.Count() == 10 );
		CHECK( std::equal(v.begin(), v.end(), array) );
	}
	{
		const int value = 127;
		const size_t count = 24;
		BaseArray v(count, value);

		CHECK( v.Count() == count );

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

		CHECK( v.Count() == other.Count() );
		CHECK( std::equal(v.begin(), v.end(), other.begin()) );
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

TEST_CASE("Array_Assignment") {
//	typedef tinystl::vector<int> vector;
//
//	{
//		const int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
//		vector other(array, array + 10);
//
//		vector v;
//		v = other;
//
//		CHECK( v.Count() == 10 );
//		CHECK( std::equal(v.begin(), v.end(), array) );
//		CHECK( other.Count() == 10 );
//		CHECK( std::equal(v.begin(), v.end(), other.begin()) );
//	}
}

TEST_CASE("Array_Pushback") {
//	tinystl::vector<int> v;
//	v.push_back(42);
//
//	CHECK(v.Count() == 1);
//	CHECK(v[0] == 42);
}

TEST_CASE("Array_Array") {
//	tinystl::vector< tinystl::vector<int> > v(10, tinystl::vector<int>());
//
//	tinystl::vector< tinystl::vector<int> >::iterator it = v.begin(), end = v.end();
//	for (; it != end; ++it) {
//		CHECK( (*it).empty() );
//		CHECK( (*it).Count() == 0 );
//		CHECK( (*it).begin() == (*it).end() );
//	}
}

TEST_CASE("Vector_Swap") {
//	tinystl::vector<int> v1;
//	v1.push_back(12);
//	v1.push_back(20);
//
//	tinystl::vector<int> v2;
//	v2.push_back(54);
//
//	v1.swap(v2);
//
//	CHECK(v1.Count() == 1);
//	CHECK(v2.Count() == 2);
//	CHECK(v1[0] == 54);
//	CHECK(v2[0] == 12);
//	CHECK(v2[1] == 20);
}

TEST_CASE("Array_PopBack") {
//	tinystl::vector<int> v;
//	v.push_back(12);
//	v.push_back(24);
//
//	CHECK(v.back() == 24);
//
//	v.pop_back();
//
//	CHECK(v.back() == 12);
//	CHECK(v.Count() == 1);
}

//TEST_CASE(vector_assign) {
//	tinystl::vector<int> v;
//
//	CHECK(v.Count() == 0);
//
//	const int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
//	v.assign(array, array + 10);
//	CHECK(v.Count() == 10);
//	CHECK( std::equal(v.begin(), v.end(), array) );
//}
//
//TEST_CASE(vector_erase) {
//	const int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
//	tinystl::vector<int> v(array, array + 10);
//
//	tinystl::vector<int>::iterator it = v.erase(v.begin());
//	CHECK(*it == 2);
//	CHECK(v.Count() == 9);
//	CHECK( std::equal(v.begin(), v.end(), array + 1) );
//
//	it = v.erase(v.end() - 1);
//	CHECK(it == v.end());
//	CHECK(v.Count() == 8);
//	CHECK( std::equal(v.begin(), v.end(), array + 1) );
//
//	v.erase(v.begin() + 1, v.end() - 1);
//	CHECK(v.Count() == 2);
//	CHECK(v[0] == 2);
//	CHECK(v[1] == 9);
//}
//
//TEST_CASE(vector_erase_unordered) {
//	const int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
//	typedef tinystl::vector<int> vector;
//	vector v(array, array + 10);
//
//	int first = *(v.begin());
//	vector::iterator it = v.erase_unordered(v.begin());
//	CHECK(it == v.begin());
//	CHECK(v.Count() == 9);
//	CHECK( std::count(v.begin(), v.end(), first) == 0 );
//	for (it = v.begin(); it != v.end(); ++it) {
//		CHECK( std::count(v.begin(), v.end(), *it) == 1 );
//	}
//
//	int last = *(v.end() - 1);
//	it = v.erase_unordered(v.end() - 1);
//	CHECK(it == v.end());
//	CHECK(v.Count() == 8);
//	CHECK( std::count(v.begin(), v.end(), last) == 0 );
//	for (it = v.begin(); it != v.end(); ++it) {
//		CHECK( std::count(v.begin(), v.end(), *it) == 1 );
//	}
//
//	first = *(v.begin());
//	last = *(v.end() - 1);
//	v.erase_unordered(v.begin() + 1, v.end() - 1);
//	CHECK(v.Count() == 2);
//	CHECK( std::count(v.begin(), v.end(), first) == 1 );
//	CHECK( std::count(v.begin(), v.end(), last) == 1 );
//}
//
//TEST_CASE(vector_insert) {
//	const int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
//	tinystl::vector<int> v(array, array + 10);
//
//	v.insert(v.begin(), 0);
//	CHECK(v.Count() == 11);
//	CHECK(v[0] == 0);
//	CHECK( std::equal(v.begin() + 1, v.end(), array) );
//
//	v.insert(v.end(), 11);
//	CHECK(v.Count() == 12);
//	CHECK(v[0] == 0);
//	CHECK( std::equal(array, array + 10, v.begin() + 1) );
//	CHECK(v.back() == 11);
//
//	const int array2[3] = {11, 12, 13};
//	const int finalarray[] = {0, 1, 2, 3, 11, 12, 13, 4, 5, 6, 7, 8, 9, 10, 11};
//	v.insert(v.begin() + 4, array2, array2 + 3);
//	CHECK( v.Count() == 15 );
//	CHECK( std::equal(v.begin(), v.end(), finalarray) );
//}
//
//TEST_CASE(vector_iterator) {
//	const int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
//
//	tinystl::vector<int> v(array, array + 10);
//	const tinystl::vector<int>& cv = v;
//
//	CHECK(v.data() == &*v.begin());
//	CHECK(v.data() == &v[0]);
//	CHECK(v.data() + v.Count() == &*v.end());
//	CHECK(v.begin() == cv.begin());
//	CHECK(v.end() == cv.end());
//	CHECK(v.data() == cv.data());
//
//	tinystl::vector<int> w = v;
//	CHECK(v.begin() != w.begin());
//	CHECK(v.end() != w.end());
//}