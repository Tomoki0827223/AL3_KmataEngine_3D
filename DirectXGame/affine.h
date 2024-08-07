#pragma once
#include "MathUtilityForText.h"
#include "math.h"
#include <cmath>
#include <numbers>
#include "Matrix4x4.h"

Matrix4x4 MatrixMultipry(const Matrix4x4& matrix1, const Matrix4x4& matrix2);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

Matrix4x4 MakeRotateZMatrix(float angle);

Vector3 Transform(const Vector3& vec, const Matrix4x4& mat);