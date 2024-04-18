#pragma once

#include "Vector3.h"

/// <summary>
/// 点光源
/// </summary>
class PointLight {
public: // サブクラス
	// 定数バッファ用データ構造体
	struct ConstBufferData {
		Vector3 lightpos;
		float pad1;
		Vector3 lightcolor;
		float pad2;
		Vector3 lightatten;
		unsigned int active;
	};

public: // メンバ関数
	/// <summary>
	/// ライト座標をセット
	/// </summary>
	/// <param name="lightpos">ライト座標</param>
	inline void SetLightPos(const Vector3& lightpos) { lightPos_ = lightpos; }

	/// <summary>
	/// ライト座標を取得
	/// </summary>
	/// <returns>ライト座標</returns>
	inline const Vector3& GetLightPos() const { return lightPos_; }

	/// <summary>
	/// ライト色をセット
	/// </summary>
	/// <param name="lightcolor">ライト色</param>
	inline void SetLightColor(const Vector3& lightcolor) { lightColor_ = lightcolor; }

	/// <summary>
	/// ライト色を取得
	/// </summary>
	/// <returns>ライト色</returns>
	inline const Vector3& GetLightColor() const { return lightColor_; }

	/// <summary>
	/// ライト距離減衰係数をセット
	/// </summary>
	/// <param name="lightatten">ライト距離減衰係数</param>
	inline void SetLightAtten(const Vector3& lightAtten) { lightAtten_ = lightAtten; }

	/// <summary>
	/// ライト距離減衰係数を取得
	/// </summary>
	/// <returns>ライト距離減衰係数</returns>
	inline const Vector3& GetLightAtten() const { return lightAtten_; }

	/// <summary>
	/// 有効フラグをセット
	/// </summary>
	/// <param name="active">有効フラグ</param>
	inline void SetActive(bool active) { active_ = active; }

	/// <summary>
	/// 有効チェック
	/// </summary>
	/// <returns>有効フラグ</returns>
	inline bool IsActive() const { return active_; }

private: // メンバ変数
	// ライト座標（ワールド座標系）
	Vector3 lightPos_ = {0, 0, 0};
	// ライト色
	Vector3 lightColor_ = {1, 1, 1};
	// ライト距離減衰係数
	Vector3 lightAtten_ = {1.0f, 1.0f, 1.0f};
	// 有効フラグ
	bool active_ = false;
};
