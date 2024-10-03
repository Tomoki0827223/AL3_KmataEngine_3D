#pragma once

/// <summary>
/// 3次元ベクトル
/// </summary>
/// 
class Vector3 {
public:
	float x, y, z;

	Vector3 operator+(const Vector3& other) const { return Vector3{x + other.x, y + other.y, z + other.z}; }

	// 他の演算子や関数も必要に応じて追加する

	// コンストラクタやその他の関数の定義もここに含まれる
};

