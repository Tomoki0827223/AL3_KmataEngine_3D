#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include <cmath>

/// <summary>
/// スポットライト
/// </summary>
class SpotLight {
public: // サブクラス
	// 定数バッファ用データ構造体
	struct ConstBufferData {
		Vector3 lightv;
		float pad1;
		Vector3 lightpos;
		float pad2;
		Vector3 lightcolor;
		float pad3;
		Vector3 lightatten;
		float pad4;
		Vector2 lightfactoranglecos;
		unsigned int active;
		float pad5;
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
	/// ライト座標をセット
	/// </summary>
	/// <param name="lightpos">ライト座標</param>
	inline void SetLightPos(const Vector3& lightpos) { this->lightPos_ = lightpos; }

	/// <summary>
	/// ライト座標を取得
	/// </summary>
	/// <returns>ライト座標</returns>
	inline const Vector3& GetLightPos() const { return lightPos_; }

	/// <summary>
	/// ライト色をセット
	/// </summary>
	/// <param name="lightcolor">ライト色</param>
	inline void SetLightColor(const Vector3& lightcolor) { this->lightColor_ = lightcolor; }

	/// <summary>
	/// ライト色を取得
	/// </summary>
	/// <returns>ライト色</returns>
	inline const Vector3& GetLightColor() const { return lightColor_; }

	/// <summary>
	/// ライト距離減衰係数をセット
	/// </summary>
	/// <param name="lightatten">ライト距離減衰係数</param>
	inline void SetLightAtten(const Vector3& lightAtten) { this->lightAtten_ = lightAtten; }

	/// <summary>
	/// ライト距離減衰係数を取得
	/// </summary>
	/// <returns>ライト距離減衰係数</returns>
	inline const Vector3& GetLightAtten() const { return lightAtten_; }

	/// <summary>
	/// ライト減衰角度をセット
	/// </summary>
	/// <param name="lightFactorAngle">x:減衰開始角度 y:減衰終了角度[radian]</param>
	inline void SetLightFactorAngle(const Vector2& lightFactorAngle) {
		this->lightFactorAngleCos_.x = std::cos(lightFactorAngle.x);
		this->lightFactorAngleCos_.y = std::cos(lightFactorAngle.y);
	}

	/// <summary>
	/// ライト減衰角度を取得
	/// </summary>
	/// <returns>ライト距離減衰係数</returns>
	inline const Vector2& GetLightFactorAngleCos() const { return lightFactorAngleCos_; }

	/// <summary>
	/// 有効フラグをセット
	/// </summary>
	/// <param name="active">有効フラグ</param>
	inline void SetActive(bool active) { this->active_ = active; }

	/// <summary>
	/// 有効チェック
	/// </summary>
	/// <returns>有効フラグ</returns>
	inline bool IsActive() const { return active_; }

private: // メンバ変数
	// ライト方向（単位ベクトル）
	Vector3 lightDir_ = {1, 0, 0};
	// ライト座標（ワールド座標系）
	Vector3 lightPos_ = {0, 0, 0};
	// ライト色
	Vector3 lightColor_ = {1, 1, 1};
	// ライト距離減衰係数
	Vector3 lightAtten_ = {1.0f, 1.0f, 1.0f};
	// ライト減衰角度
	Vector2 lightFactorAngleCos_ = {0.2f, 0.5f};
	// 有効フラグ
	bool active_ = false;
};
