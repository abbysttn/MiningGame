// COMP710 GP Framework 2022
#include "matrix4.h"

void SetZero(Matrix4& mat)
{
	mat.m[0][0] = 0.0f;
	mat.m[0][1] = 0.0f;
	mat.m[0][2] = 0.0f;
	mat.m[0][3] = 0.0f;

	mat.m[1][0] = 0.0f;
	mat.m[1][1] = 0.0f;
	mat.m[1][2] = 0.0f;
	mat.m[1][3] = 0.0f;

	mat.m[2][0] = 0.0f;
	mat.m[2][1] = 0.0f;
	mat.m[2][2] = 0.0f;
	mat.m[2][3] = 0.0f;

	mat.m[3][0] = 0.0f;
	mat.m[3][1] = 0.0f;
	mat.m[3][2] = 0.0f;
	mat.m[3][3] = 0.0f;
}

void SetIdentity(Matrix4& mat)
{
	SetZero(mat);

	mat.m[0][0] = 1.0f;
	mat.m[1][1] = 1.0f;
	mat.m[2][2] = 1.0f;
	mat.m[3][3] = 1.0f;
}

void CreateOrthoProjection(Matrix4& mat, float width, float height)
{
	SetZero(mat);

	mat.m[0][0] = 2.0f / width;
	mat.m[1][1] = 2.0f / -height;
	mat.m[2][2] = -2.0f / (1.0f - -1.0f);

	mat.m[3][0] = -1.0f;
	mat.m[3][1] = 1.0f;
	mat.m[3][2] = 0.0f;
	mat.m[3][3] = 1.0f;
}
Matrix4 Matrix4::operator*(const Matrix4& other) const
{
	Matrix4 result;
	SetZero(result); // Use your existing helper

	for (int row = 0; row < 4; ++row)
	{
		for (int col = 0; col < 4; ++col)
		{
			for (int k = 0; k < 4; ++k)
			{
				result.m[row][col] += m[row][k] * other.m[k][col];
			}
		}
	}

	return result;
}