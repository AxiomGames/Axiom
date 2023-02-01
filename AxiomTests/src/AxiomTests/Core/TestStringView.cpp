
#include "Axiom/Core/StringView.hpp"
#include "doctest.h"


TEST_CASE("TestStringView_Constructor")
{
    StringView stringView{"asd"};
    CHECK(!stringView.Empty());
    CHECK(stringView.Size() == 3);
    CHECK(stringView == "asd");

	int count = 0;
	for(auto c: stringView) {
		CHECK(c > 0);
		count++;
	}
	CHECK(count == 3);
}

void StrStringView(const StringView& strV) {
	CHECK(strV.Size() == 5);
	CHECK(strV == "abcde");
}

TEST_CASE("from string") {
	String myStr{"abcde"};
	StrStringView(myStr);
}


TEST_CASE("TestStringView_Hash")
{
	StringView stringView{"asd"};
	auto vl = HashValue(stringView);
	CHECK(vl > 0);
}

