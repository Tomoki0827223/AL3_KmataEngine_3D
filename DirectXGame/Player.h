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

	void CheckMapCollision(CollisionMapInfo& info);
	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);


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
//
//
//#pragma once
//
//#include "Model.h"
//#include "TurnController.h"
//#include "WorldTransform.h" // 実際のヘッダーファイル名に修正する必要があります
//#include <algorithm>
//#include <array>
//
//enum class LRDirection {
//	kRight,
//	kLeft,
//};
//
//class MapChipField; // MapChipField クラスの宣言が必要です
//
//struct CollisionMapInfo {
//	bool hitCeilingFlag = false;
//	bool landingFlag = false;
//	bool wallContactFlag = false;
//	Vector3 movement;
//};
//
//enum Corner { kRightBottom, kLeftBottom, kRightTop, kLeftTop, kNumCorner };
//
//class Player {
//public:
//	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);
//	void Update();
//	void Draw();
//	void SetMapChipField(MapChipField* mapchipField) { mapChipField_ = mapchipField; }
//	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
//	const Vector3& GetVelocity() const { return velocity_; }
//
//private:
//	void HandleCeilingCollision(const CollisionMapInfo& info);
//	void HandleWallCollision(const CollisionMapInfo& info); // 追加
//
//	void ApplyCollisionResultAndMove(const CollisionMapInfo& info);
//	void CheckMapCollision(CollisionMapInfo& info);
//
//	void CheckMapCollisionUp(CollisionMapInfo& info);
//	void CheckMapCollisionDown(CollisionMapInfo& info);
//	void CheckMapCollisionRight(CollisionMapInfo& info);
//	void CheckMapCollisionLeft(CollisionMapInfo& info);
//
//	Vector3 CornerPosition(const Vector3& center, Corner corner);
//
//	WorldTransform worldTransform_;
//	ViewProjection* viewProjection_ = nullptr;
//	Model* model_ = nullptr;
//	uint32_t textureHandle_ = 0u;
//	MapChipField* mapChipField_ = nullptr;
//
//	bool onGround_ = true;
//	LRDirection lrDirection_ = LRDirection::kRight;
//	float turnFirstRotationY_ = 0.0f;
//	float turnTimer_ = 0.0f;
//	Vector3 velocity_ = {};
//
//	TurnController turnController_;
//
//	static inline const float kAcceleration = 0.1f;
//	static inline const float kLimitRunSpeed = 1.0f;
//	static inline const float kAttenuation = 0.1f;
//	static inline const float kTimeTurn = 1.0f;
//	static inline const float kVelocityBias = 0.1f;
//	static inline const float kGravityAcceleration = 0.2f;
//	static inline const float kLimitFallSpeed = -2.0f;
//	static inline const float kJumpAcceleration = 1.0f;
//	static inline const float kWidth = 1.0f;
//	static inline const float kHeight = 1.0f;
//	static inline const float kBlank = 0.1f; // 適切な値に修正する
//};
