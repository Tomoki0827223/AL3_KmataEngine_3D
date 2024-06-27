#pragma once

#include <Model.h>
#include <WorldTransform.h>

enum class LRDirection {

	kRight,
	kLeft,
};

class Player {

public:

	bool onGround_ = true;
	LRDirection lrDirection_ = LRDirection::kRight;
	static inline const float kAcceleration = 0.05f;
	static inline const float kLimitRunSpeed = 1.0f;
	static inline const float kAttenuation = 0.1f;
	static inline const float kTimeTurn = 0.3f;

	// 重力加速度(下方向)
	static inline const float kGravityAcceleration = 0.5f;
	// 最大落下速度(下方向)
	static inline const float kLimitFallSpeed = 0.5f;
	// ジャンプ初速(上方向)
	static inline const float kJumpAcceleration = 1.5f;

	float turnFirstRotesionY_ = 0.0f;
	float turnTimer_ = 0.0f;

	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	const Vector3& GetVelocity() const { return velocity_; }
	const Vector3& GetPlayerPosition() const { return worldTransform_.translation_; }

	Vector3 velocity_ = {};

	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	void Update();

	void Draw();


private:
	WorldTransform worldTransform_;

	ViewProjection* viewProjection_ = nullptr;

	Model* model_ = nullptr;

	uint32_t textureHandle_ = 0u;
};
