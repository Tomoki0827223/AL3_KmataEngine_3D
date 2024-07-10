#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include "TurnController.h"
#include <algorithm>
#include <array>

class MapChipField; // MapChipField クラスの宣言が必要です

class Player {
public:

	// 左右
	enum class LRDirection {
		kRight,
		kLeft,
	};

	// 角
	struct CollisionMapInfo {
		bool hitCeilingFlag = false;
		bool landingFlag = false;
		bool wallContactFlag = false;
		Vector3 movement;
	};

	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	enum Corner { kRightBottom, kLeftBottom, kRightTop, kLeftTop, kNumCorner };

	void Update();

	void MovePlayer();
	
	void Draw();

	//seteer
	void SetMapChipField(MapChipField* mapchipField) { mapChipField_ = mapchipField; }
	//geter
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const Vector3& GetVelocity() const { return velocity_; }

	static inline const float kAcceleration = 0.1f;
	static inline const float kAttenuation = 0.05f;
	static inline const float kJumpAcceleration = 5.0f;
	static inline const float kGravityAcceleration = 0.98f;
	static inline const float kAttenuationWall = 0.2f; 
	static inline const float kAttenuationLanding = 0.0f;
	static inline const float kLimitFallSpeed = 0.5f;
	static inline const float kLimitRunSpeed = 0.5f;
	static inline const float kTimeTurn = 0.3f;
	static inline const float kWidth = 0.8f;
	static inline const float kHeight = 0.8f;
	static inline const float kBlank = 0.04f; // 適切な値に修正する
	static inline const float kGroundSearchHeight = 0.06f;

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	WorldTransform worldTransform_;
	ViewProjection* viewProjection_ = nullptr;
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	MapChipField* mapChipField_ = nullptr;

	bool onGround_ = true;
	LRDirection lrDirection_ = LRDirection::kRight;
	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;
	Vector3 velocity_ = {};

	TurnController turnController_;


	void CheckMapMove(const CollisionMapInfo& info);

	void CheckMapCollision(CollisionMapInfo& info);

	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	void TurnControll();

	void CeilingContact(const CollisionMapInfo& info);


	void JumpTranformMove(const CollisionMapInfo& info);
};
