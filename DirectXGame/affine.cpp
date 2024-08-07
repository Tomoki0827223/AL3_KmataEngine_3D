#include "affine.h"

Matrix4x4 MatrixMultipry(const Matrix4x4& matrix1, const Matrix4x4& matrix2) {
	Matrix4x4 result;

	for (int i = 0; i < 4; ++i) {

		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = matrix1.m[i][0] * matrix2.m[0][j] + matrix1.m[i][1] * matrix2.m[1][j] + matrix1.m[i][2] * matrix2.m[2][j] + matrix1.m[i][3] * matrix2.m[3][j];
		}
	}

	return result;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rot, const Vector3& translate) {

	// エラー対策(使用しない)
	Vector3 dm = scale;

	// 回転
	Matrix4x4 RotateMatY = {cosf(rot.y), 0, -sinf(rot.y), 0, 0, 1, 0, 0, sinf(rot.y), 0, cosf(rot.y), 0, 0, 0, 0, 1};

	// 平行移動行列の作成
	Matrix4x4 TranslateMat = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, translate.x, translate.y, translate.z, 1};

	// 回転*平行移動だけをワールド変換行列に
	Matrix4x4 ansMat = MatrixMultipry(RotateMatY, TranslateMat);

	return ansMat;
}

Matrix4x4 MakeRotateZMatrix(float angle) {
	Matrix4x4 matrix;

	float cosAngle = std::cos(angle);
	float sinAngle = std::sin(angle);

	matrix.m[0][0] = cosAngle;
	matrix.m[0][1] = -sinAngle;
	matrix.m[0][2] = 0.0f;
	matrix.m[0][3] = 0.0f;

	matrix.m[1][0] = sinAngle;
	matrix.m[1][1] = cosAngle;
	matrix.m[1][2] = 0.0f;
	matrix.m[1][3] = 0.0f;

	matrix.m[2][0] = 0.0f;
	matrix.m[2][1] = 0.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[2][3] = 0.0f;

	matrix.m[3][0] = 0.0f;
	matrix.m[3][1] = 0.0f;
	matrix.m[3][2] = 0.0f;
	matrix.m[3][3] = 1.0f;

	return matrix;
}

Vector3 Transform(const Vector3& vec, const Matrix4x4& mat) {
	Vector3 result;

	result.x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + mat.m[3][0];
	result.y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + mat.m[3][1];
	result.z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + mat.m[3][2];

	return result;
}