#pragma once

#include "DebugText.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

/// <summary>
/// 自キャラ
/// </summary>

class MapChipField;

class Player {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	float x, y, z;

	// += 演算子のオーバーロード
	Vector3& operator+=(const Vector3& other) {
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
	}

	const WorldTransform& GetWorldTransform() { return worldTransform_; }

	Vector3& GetVelocity() { return velocity_; }

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	void MoveInput();

	// マップとのあたり判定情報
	struct CollisionMapInfo {
		bool CeilingCollisionFlag = false;
		bool LandingFlag = false;
		bool WallConstactFlag = false;
		Vector3 movement_;
	};

	void MapCollision(CollisionMapInfo& info);

	void MapCollisionUp(CollisionMapInfo& info);

	void MapCollisionDown(CollisionMapInfo& info);

	void MapCollisionLeft(CollisionMapInfo& info);

	void MapCollisionRight(CollisionMapInfo& info);

	void JudgmentMove(const CollisionMapInfo& info);

	void CeilingContact(const CollisionMapInfo& info);

	void TurnControll();

	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	// 角
	enum Corner {
		kRightBottom, // 右下
		kLeftBottom,  // 左下
		kRightTop,    // 右上
		kLeftTop,     // 左上

		kNumCorner // 要素数
	};

	Vector3 CornerPosition(const Vector3& center, Corner corner);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	ViewProjection* viewProjection_ = nullptr;

	Vector3 velocity_ = {};
	static inline const float kAcceleration = 0.01f;
	static inline const float kAttenuation = 0.2f;
	static inline const float kLimitRunSpeed = 1.0f;

	LRDirection lrDirection_ = LRDirection::kRight;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.5f;

	// 接地状態フラグ
	bool onGround_ = true;
	// 重力加速度
	static inline const float kGravityAcceleration = 0.05f;
	// 最大落下速度
	static inline const float kLimitFallSpeed = 1.0f;
	// ジャンプ初速
	static inline const float kJumpAcceleration = 0.5f;

	MapChipField* mapChipField_ = nullptr;
	// キャラクターのあたり判定サイズ
	static inline const float kWidth = 0.8f;

	static inline const float kHeight = 0.8f;

	static inline const float kBlank = 1.0f;
};
