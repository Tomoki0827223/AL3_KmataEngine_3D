#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include <cmath>

/// <summary>
/// 丸影
/// </summary>
class CircleShadow {
public: // サブクラス
	// 定数バッファ用データ構造体
	struct ConstBufferData {
		Vector3 dir;
		float pad1;
		Vector3 casterPos;
		float distanceCasterLight;
		Vector3 atten;
		float pad2;
		Vector2 factorAngleCos;
		unsigned int active;
		float pad3;
	};

public: // メンバ関数
	/// <summary>
	/// 方向をセット
	/// </summary>
	/// <param name="lightdir">方向</param>
	void SetDir(const Vector3& dir);

	/// <summary>
	/// ライト方向を取得
	/// </summary>
	/// <returns>ライト方向</returns>
	inline const Vector3& GetDir() const { return dir_; }

	/// <summary>
	/// キャスター座標をセット
	/// </summary>
	/// <param name="lightpos">キャスター座標</param>
	inline void SetCasterPos(const Vector3& casterPos) { casterPos_ = casterPos; }

	/// <summary>
	/// キャスター座標を取得
	/// </summary>
	/// <returns>キャスター座標</returns>
	inline const Vector3& GetCasterPos() const { return casterPos_; }

	/// <summary>
	/// キャスターとライトの距離をセット
	/// </summary>
	/// <param name="lightpos">キャスターとライトの距離</param>
	inline void SetDistanceCasterLight(float distanceCasterLight) {
		distanceCasterLight_ = distanceCasterLight;
	}

	/// <summary>
	/// キャスターとライトの距離を取得
	/// </summary>
	/// <returns>キャスターとライトの距離</returns>
	inline float GetDistanceCasterLight() const { return distanceCasterLight_; }

	/// <summary>
	/// 距離減衰係数をセット
	/// </summary>
	/// <param name="lightatten">ライト距離減衰係数</param>
	inline void SetAtten(const Vector3& atten) { atten_ = atten; }

	/// <summary>
	/// 距離減衰係数を取得
	/// </summary>
	/// <returns>ライト距離減衰係数</returns>
	inline const Vector3& GetAtten() const { return atten_; }

	/// <summary>
	/// 減衰角度をセット
	/// </summary>
	/// <param name="lightFactorAngle">x:減衰開始角度 y:減衰終了角度[radian]</param>
	inline void SetFactorAngle(const Vector2& factorAngle) {
		factorAngleCos_.x = std::cos(factorAngle.x);
		factorAngleCos_.y = std::cos(factorAngle.y);
	}

	/// <summary>
	/// 減衰角度を取得
	/// </summary>
	/// <returns>減衰角度</returns>
	inline const Vector2& GetFactorAngleCos() const { return factorAngleCos_; }

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
	// 方向（単位ベクトル）
	Vector3 dir_ = {1, 0, 0};
	// キャスターとライトの距離
	float distanceCasterLight_ = 100.0f;
	// キャスター座標（ワールド座標系）
	Vector3 casterPos_ = {0, 0, 0};
	// 距離減衰係数
	Vector3 atten_ = {0.5f, 0.6f, 0.0f};
	// 減衰角度
	Vector2 factorAngleCos_ = {0.2f, 0.5f};
	// 有効フラグ
	bool active_ = false;
};
