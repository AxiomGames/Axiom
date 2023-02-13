
#include "Axiom/Core/Reflection.hpp"
#include "doctest.h"


struct TestReflectionType
{
	int a{};
	float b{};


	float Multiply(float v) const
	{
		return b * v;
	};
};

TEST_CASE("Reflection_Register")
{

	//registry
	{
		auto testReflectionType = Reflection::NewType<TestReflectionType>("TestReflectionType");
		testReflectionType.NewFunction<&TestReflectionType::Multiply>("Multiply");
	}

	//runtime
	{
		auto testReflectionType = Reflection::FindType("TestReflectionType");
		REQUIRE(testReflectionType);

		auto multiply = testReflectionType.FindFunction("Multiply");
		REQUIRE(multiply);


		//TODO change to testReflectionType.NewInstance(10, 20);
		TestReflectionType reflectionType{10, 20};


		auto ret = multiply.Invoke<float>(&reflectionType, 2.f);
		CHECK(ret == 40);
	}


}

enum class EnumClassTest
{
	One = 1,
	Two = 5,
	Three = 3
};

TEST_CASE("Reflection_Register_Enum")
{
	//registry
	{
		auto enumClassTest = Reflection::NewType<EnumClassTest>("EnumClassTest");
		enumClassTest.Value<EnumClassTest::One>("One");
		enumClassTest.Value<EnumClassTest::Two>("Two");
		enumClassTest.Value<EnumClassTest::Three>("Three");
	}

	//runtime
	{
		auto typeEnumClassTest = Reflection::FindType("EnumClassTest");
		//by Name
		{
			auto value = typeEnumClassTest.FindValueByName("Two");
			REQUIRE(value);
			CHECK(value.As<EnumClassTest>() == EnumClassTest::Two);
			CHECK(value.As<int>() == 5);
			CHECK(value.GetName() == "Two");
		}

		//by Value
		{
			auto three = typeEnumClassTest.FindValue(EnumClassTest::Three);
			REQUIRE(three);

			CHECK(three.As<EnumClassTest>() == EnumClassTest::Three);
			CHECK(three.As<int>() == 3);
			CHECK(three.GetName() == "Three");
		}
		{
			auto values = typeEnumClassTest.Values();
			REQUIRE(values[0]);
			REQUIRE(values[1]);
			REQUIRE(values[2]);

			CHECK(values[0].As<EnumClassTest>() == EnumClassTest::One);
			CHECK(values[1].As<EnumClassTest>() == EnumClassTest::Two);
			CHECK(values[2].As<EnumClassTest>() == EnumClassTest::Three);
		}

	}

}