#pragma once
#include "Math.hpp"

AMATH_NAMESPACE

struct Color32
{
	union
	{
		struct { byte r, g, b, a; };
		byte arr[4];
		int _int;
	};
	Color32() : r(0), g(0), b(0), a(0) {};
	Color32(__int32 __int) : _int(__int) {};
	Color32(byte _r, byte _g, byte _b) : r(_r), g(_g), b(_b), a(255) {};
	Color32(byte _r, byte _g, byte _b, byte _a) : r(_r), g(_g), b(_b), a(_a) {};

	[[nodiscard]] static FINLINE Color32 Red()	 { return Color32(255, 0, 0, 255); }
	[[nodiscard]] static FINLINE Color32 Green() { return Color32(0, 255, 0, 255); }
	[[nodiscard]] static FINLINE Color32 Blue()	 { return Color32(0, 0, 255, 255); }
	[[nodiscard]] static FINLINE Color32 Orange(){ return Color32(128, 128, 0, 255); }
};

struct Color
{
	union
	{
		struct { float r, g, b, a; };
		float arr[4];
		__m128 vec;
	};

	static constexpr float OneDiv255 = 1.0f / 255.0f;

	FINLINE Color() : r(0), g(0), b(0), a(0) {}
	FINLINE Color(float fill) : r(fill), g(fill), b(fill), a(fill) {}
	FINLINE Color(Color32 col) : r(col.r * OneDiv255), g(col.g * OneDiv255), b(col.b * OneDiv255), a(col.a * OneDiv255) {}
	FINLINE Color(float _r, float _g, float _b) : r(_r), g(_g), b(_b), a(1) {}
	Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
	Color(__m128 _vec) : vec(_vec) {}

	FINLINE static float Dot(const Color& a, const Color& b)
	{
		return a.arr[0] * b.arr[0] + a.arr[1] * b.arr[1] + a.arr[2] * b.arr[2];
	}

	// lerp
	FINLINE static Color VECTORCALL Mix(Color a, Color b, const float t)
	{
		b.a = 1;				
		b -= a;					
		b *= Color(t);			
		return a + b;
	}

	FINLINE static Color VECTORCALL MixWithA(Color a, Color b, const float t)
	{
		b -= a;
		b *= Color(t);
		return a + b;
	}

	Color32 ConvertToColor32() {
		Color converted = *this * 255.0f;
		return Color32(byte(converted.r), byte(converted.g), byte(converted.b));
	}

	FINLINE Color VECTORCALL operator + (const Color b) const { return _mm_add_ps(vec, b.vec); }
	FINLINE Color VECTORCALL operator - (const Color b) const { return _mm_sub_ps(vec, b.vec); }
	FINLINE Color VECTORCALL operator * (const Color b) const { return _mm_mul_ps(vec, b.vec); }
	FINLINE Color VECTORCALL operator / (const Color b) const { return _mm_div_ps(vec, b.vec); }

	FINLINE Color VECTORCALL operator += (const Color b) { vec = _mm_add_ps(vec, b.vec); return *this; }
	FINLINE Color VECTORCALL operator -= (const Color b) { vec = _mm_sub_ps(vec, b.vec); return *this; }
	FINLINE Color VECTORCALL operator *= (const Color b) { vec = _mm_mul_ps(vec, b.vec); return *this; }
	FINLINE Color VECTORCALL operator /= (const Color b) { vec = _mm_div_ps(vec, b.vec); return *this; }

	FINLINE Color operator *  (const float b) const { return _mm_mul_ps(vec, _mm_set_ps1(b)); }
	FINLINE Color operator /  (const float b) const { return _mm_div_ps(vec, _mm_set_ps1(b)); }
	FINLINE Color operator *= (const float b) { vec = _mm_mul_ps(vec, _mm_set_ps1(b)); return *this; }
	FINLINE Color operator /= (const float b) { vec = _mm_div_ps(vec, _mm_set_ps1(b)); return *this; }

	static FINLINE Color Red()		{ return Color(1.0, 0.0, 0.0, 1.0); }
	static FINLINE Color Green()	{ return Color(0.0, 1.0, 0.0, 1.0); }
	static FINLINE Color Blue()		{ return Color(0.0, 0.0, 1.0, 1.0); }
	static FINLINE Color Orange()	{ return Color(0.5, 0.5, 0.0, 1.0); }
};

AMATH_END_NAMESPACE