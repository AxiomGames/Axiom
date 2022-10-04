#pragma once
#include "Quaternion.hpp"

struct Matrix3
{
	union
	{
		float m[3][3];
		Vector3f vec[3];
	};

	inline static Matrix3 LookAt(Vector3f direction, Vector3f up)
	{
		Matrix3 result;
		result.vec[2] = direction;
		Vector3f const& Right = Vector3f::Cross(up, result.vec[2]);
		result.vec[0] = Right * rsqrt(max(0.00001f, Vector3f::Dot(Right, Right)));
		result.vec[1] = Vector3f::Cross(result.vec[2], result.vec[0]);
		return result;
	}

	Quaternion ToQuaternion() const
	{
		float fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
		float fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
		float fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
		float fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

		int biggestIndex = 0;
		float fourBiggestSquaredMinus1 = fourWSquaredMinus1;
		if(fourXSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourXSquaredMinus1;
			biggestIndex = 1;
		}
		if(fourYSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourYSquaredMinus1;
			biggestIndex = 2;
		}
		if(fourZSquaredMinus1 > fourBiggestSquaredMinus1)
		{
			fourBiggestSquaredMinus1 = fourZSquaredMinus1;
			biggestIndex = 3;
		}

		float biggestVal = sqrt(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
		float mult = 0.25f / biggestVal;

		switch(biggestIndex)
		{
			case 0:
				return Quaternion(biggestVal, (m[1][2] - m[2][1]) * mult, (m[2][0] - m[0][2]) * mult, (m[0][1] - m[1][0]) * mult);
			case 1:
				return Quaternion((m[1][2] - m[2][1]) * mult, biggestVal, (m[0][1] + m[1][0]) * mult, (m[2][0] + m[0][2]) * mult);
			case 2:
				return Quaternion((m[2][0] - m[0][2]) * mult, (m[0][1] + m[1][0]) * mult, biggestVal, (m[1][2] + m[2][1]) * mult);
			case 3:
				return Quaternion((m[0][1] - m[1][0]) * mult, (m[2][0] + m[0][2]) * mult, (m[1][2] + m[2][1]) * mult, biggestVal);
			default: // Silence a -Wswitch-default warning in GCC. Should never actually get here. Assert is just for sanity.
				assert(false);
				return Quaternion(1, 0, 0, 0);
		}
	}
};