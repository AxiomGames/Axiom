#include <iostream>
#include "Axiom/Core/String.hpp"

int main()
{
	Ax::String str;
	Ax::String str2("Yo");
	Ax::String str3 = str2;
	str3.Append('A');
	str3.AppendInt(32);

	std::cout << "Hello, World! " << str.Length() << ", " << str2.Length() << ", " << str3.Length() << std::endl;
	return 0;
}
