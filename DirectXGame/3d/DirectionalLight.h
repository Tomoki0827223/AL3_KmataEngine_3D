#pragma once

#include "Vector3.h"

/// <summary>
/// 平行光源
/// </summary>
class DirectionalLight {
public: // サブクラス
	// 定数バッファ用データ構造体
	struct ConstBufferData {
		Vector3 lightv;
		float pad1;
		Vector3 lightcolor;
		unsigned int active;
	};

public: // メンバ関数
	/// <summary>
	/// ライト方向をセット
	/// </summary>
	/// <param name="lightdir">ライト方向</param>
	void SetLightDir(const Vector3& lightdir);

	/// <summary>
	/// ライト方向を取得
	/// </summary>
	/// <returns>ライト方向</returns>
	inline const Vector3& GetLightDir() const { return lightDir_; }

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
	// ライト方向（単位ベクトル）
	Vector3 lightDir_ = {1, 0, 0};
	// ライト色
	Vector3 lightColor_ = {1, 1, 1};
	// 有効フラグ
	bool active_ = false;
};
