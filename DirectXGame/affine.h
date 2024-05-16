#pragma once
#include <GameScene.h>

Matrix4x4 MatrixMultipry(const Matrix4x4& matrix1, const Matrix4x4& matrix2);

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

Matrix4x4 MakeScaleMatrix(const Vector3& scale);

Matrix4x4 MakeTranslateMatrix(const Vector3& translate);

Matrix4x4 MakeRotateMatrix(const Vector3& rotate);
