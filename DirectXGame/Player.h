#pragma once

#include <Model.h>
#include <WorldTransform.h>

enum class LRDirection {
	kRight,
	kLeft,
};

class MapChipField;

// マップとの当たり判定情報
struct CollisionMapInfo {
	bool hitCeilingFlag = false;
	bool landingFlag = false;
	bool wallContactFlag = false;
	Vector3 movement;
};

// 角
enum Corner { kRightBottom, kLeftBottom, kRightTop, kLeftTop, kNumCorner };

class Player {
public:

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);
	void Update();
	void Draw();
	void SetMapChipField(MapChipField* mapchipField) { mapChipField_ = mapchipField; }
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }
	const Vector3& GetVelocity() const { return velocity_; }

private:
	void HandleCeilingCollision(const CollisionMapInfo& info);
	void HandleWallCollision(const CollisionMapInfo& info); // 追加
	
	void ApplyCollisionResultAndMove(const CollisionMapInfo& info);
	
	void CheckMapCollision(CollisionMapInfo& info);


	void CheckMapCollisionUp(CollisionMapInfo& info);
	void CheckMapCollisionDown(CollisionMapInfo& info);
	void CheckMapCollisionRight(CollisionMapInfo& info);
	void CheckMapCollisionLeft(CollisionMapInfo& info);

	WorldTransform worldTransform_;
	ViewProjection* viewProjection_ = nullptr;
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	MapChipField* mapChipField_ = nullptr;

	bool onGround_ = true;
	LRDirection lrDirection_ = LRDirection::kRight;
	float turnFirstRotesionY_ = 0.0f;
	float turnTimer_ = 0.0f;
	Vector3 velocity_ = {};

	static inline const float kAcceleration = 0.1f;
	static inline const float kLimitRunSpeed = 1.0f;
	static inline const float kAttenuation = 0.1f;
	static inline const float kTimeTurn = 1.0f;
	static inline const float kVelocityBias = 0.1f;
	static inline const float kGravityAcceleration = 0.2f;
	static inline const float kLimitFallSpeed = -2.0f;
	static inline const float kJumpAcceleration = 1.0f;
	static inline const float kWidth = 1.0f;
	static inline const float kHeight = 1.0f;
	static inline const float kBlank = 0.1f; // 適切な値に修正する
};
