
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
