#pragma once

#include <vector>
#include <string>

struct EnumValue
{
	std::string Name;
	std::string DisplayName;
	int64_t NumericalValue = -1;
};

struct Enum
{
	std::string Name;
	std::string FullNamespaceName;
	std::vector<EnumValue> Values;
};

struct ObjectProperty
{
	std::string Name;
	std::string DisplayName;
	std::string TypeName;
	uint64_t Offset = 0;
	uint64_t Size = 0;
};

struct ObjectMethod
{
	std::string Name;
	uint64_t Offset = 0;
	uint64_t ArgCount = 0;
};

struct Object
{
	std::string Name;

	std::vector<ObjectProperty> Properties;
	std::vector<ObjectMethod> Methods;
};