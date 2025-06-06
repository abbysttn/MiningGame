// COMP710 GP Framework 2025

#ifndef __MATRIX4_H
#define __MATRIX4_H
struct Matrix4
{
	float m[4][4];
	Matrix4 operator*(const Matrix4& other) const;
};
void SetZero(Matrix4& mat);
void SetIdentity(Matrix4& mat);
void CreateOrthoProjection(Matrix4& mat, float width, float height);

#endif // MATRIX4_H