#pragma once

#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
//#include "DebugText.h"
#include "AABB.h"

class MapChipField; // MapChipField クラスの宣言が必要です
class Enemy;

class Player {
public:

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

		// 角
	struct CollisionMapInfo {
		bool hitCeilingFlag = false;
		bool landingFlag = false;
		bool wallContactFlag = false;
		Vector3 movement;
	};

	void Initialize(Model* model,ViewProjection* viewProjection,const Vector3& position);

	void Update();

	void Draw();

	// seteer
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	//getter
	const WorldTransform& GetWorldTransform() { return worldTransform_; }
	Vector3& GetVelocity() {return velocity_; }

	//2-10
	//  ワールド座標を取得する関数
	Vector3 GetWorldPosition();
	// AABBを取得する関数
	AABB GetAABB();
	// 衝突応答
	void OnCollision(const Enemy* enemy);

	void MovePlayer();

	void CheckMapCollision(CollisionMapInfo& info);
	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);

	void JudgmentMove(const CollisionMapInfo& info);

	void CeilingContact(const CollisionMapInfo& info);

	void GraundSetting(const CollisionMapInfo& info);

	void TurnControll();

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	bool IsDead() const { return isDead_; }

private:

	Vector3 worldPos_;
	float radius_;
	
	bool onGround_ = true;

	bool isDead_ = false;

	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	ViewProjection* viewProjection_ = nullptr;

	Vector3 velocity_ = {};

	LRDirection lrDirection_ = LRDirection::kRight;
	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;

	MapChipField* mapChipField_ = nullptr;

	// プレイヤーの移動加速度
	static inline const float kAcceleration = 0.1f;
	// プレイヤーが停止する際の減速率
	static inline const float kAttenuation = 0.2f;
	// ジャンプ時の加速度
	static inline const float kJumpAcceleration = 0.6f;
	// 重力による加速度
	static inline const float kGravityAcceleration = 0.05f;
	// 壁に衝突した際の減速率
	static inline const float kAttenuationWall = 0.2f;
	// 着地時の減速率
	static inline const float kAttenuationLanding = 0.7f;
	// 落下速度の制限値
	static inline const float kLimitFallSpeed = 1.0f;
	// 走行速度の制限値
	static inline const float kLimitRunSpeed = 0.5f;
	// ターンするのにかかる時間
	static inline const float kTimeTurn = 0.5f;
	// プレイヤーの幅
	static inline const float kWidth = 0.8f;
	// プレイヤーの高さ
	static inline const float kHeight = 0.8f;
	// 隙間の幅（適切な値に修正する必要あり）
	static inline const float kBlank = 18.0f;
	// 地面を探す際の高さ
	static inline const float kGroundSearchHeight = 0.06f;
};

