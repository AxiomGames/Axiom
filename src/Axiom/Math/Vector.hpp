#pragma once

#include <cstdint>

namespace Ax
{
struct Vector2i
{
	union
	{
		struct
		{
			int32_t x;
			int32_t y;
		};

		struct
		{
			int32_t width;
			int32_t height;
		};
	};
};

struct Vector2
{
	float x;
	float y;
};

struct Vector3
{
	float x;
	float y;
	float z;
};

struct Vector4
{
	float x;
	float y;
	float z;
	float w;
};

struct Quaternion
{
	float x;
	float y;
	float z;
	float w;
};

struct Matrix4
{
	Vector4 m0;
	Vector4 m1;
	Vector4 m2;
	Vector4 m3;
};
}