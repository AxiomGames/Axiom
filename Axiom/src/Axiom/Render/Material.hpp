#pragma once

#include "Axiom/CoreMinimal.hpp"

AX_ENUM()
enum class EMaterialBlendMode
{
	Opaque = 0,
	Masked,
	Translucent = 2,
	Additive,
	AlphaComposite AX_META(DisplayName = "AlphaComposite (Premultiplied Alpha)"),
	AlphaComposite2 = 5 AX_META(DisplayName = "AlphaComposite2 (Premultiplied Alpha)")
};

AX_ENUM()
enum ETestEnum
{
	TestEnum_One = 0,
	TestEnum_Two  AX_META(DisplayName = "Two"),
	TestEnum_Three = 2  AX_META(DisplayName = "3")
};


namespace NMEnum
{
	enum Type
	{
		One,
		Two,
		Three
	};
}