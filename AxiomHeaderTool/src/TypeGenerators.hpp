#pragma once

#include "ReflectionTypes.hpp"

#define NL "\n"
#define WNL string += "\n"
#define LINE(line) string += std::string(line) + NL
#define LINE_S(line) string += "    " + std::string(line) + NL

static void GenerateEnum_hpp(std::string& string, const Enum& _enum)
{
	string += "template<> TypeHandler StaticType<::" + _enum.FullNamespaceName + ">();" + NL;
	WNL;
}

static void GenerateEnum_cpp(std::string& string, const Enum& _enum)
{
	LINE("static void RegisterEnum_" + _enum.FullNamespaceName + "(Reflection& reflection)");
	LINE("{");
	LINE_S("auto enumType = reflection.NewType<::" + _enum.FullNamespaceName + ">(\"" + _enum.FullNamespaceName + "\");");
	WNL;
	for (const EnumValue& enumValue : _enum.Values)
	{
		if (enumValue.DisplayName.empty() == false)
		{
			LINE_S("enumType.Value<::" + _enum.FullNamespaceName + "::" + enumValue.Name + ">(\"" + enumValue.Name + "\").Attribute(EnumAttributeNumericValue{" + std::to_string(enumValue.NumericalValue) + "}).Attribute(EnumAttributeDisplayName{\"" + enumValue.DisplayName + "\"});");
		}
		else
		{
			LINE_S("enumType.Value<::" + _enum.FullNamespaceName + "::" + enumValue.Name + ">(\"" + enumValue.Name + "\").Attribute(EnumAttributeNumericValue{" + std::to_string(enumValue.NumericalValue) + "});");
		}

		//LINE_S("enumType->Add(\"" + enumValue.Name + "\", \"" + enumValue.DisplayName + "\", " + std::to_string(enumValue.NumericalValue) + ");");
	}
	WNL;
	LINE("}");

	WNL;

	LINE("static CompileStaticRegister CompileRegisterEnum_" + _enum.Name + "(RegisterEnum_" + _enum.FullNamespaceName + ");");
	WNL;

	LINE("template<> TypeHandler StaticType<::" + _enum.FullNamespaceName + ">()");
	LINE("{");
	LINE_S("return Reflection::Get().FindType(\"" + _enum.FullNamespaceName + "\"_HASH);");
	LINE("}");
}