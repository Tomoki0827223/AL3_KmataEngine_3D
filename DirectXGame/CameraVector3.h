#pragma once

/// <summary>
/// 3次元ベクトル
/// </summary>
struct CameraVector3 final {

public:
	float x, y, z;

	// コンストラクタ
	CameraVector3() : x(0), y(0), z(0) {}
	CameraVector3(float x, float y, float z) : x(x), y(y), z(z) {}

	// 演算子オーバーロード
	CameraVector3 operator+(const CameraVector3& other) const { return CameraVector3(x + other.x, y + other.y, z + other.z); }
};
