#pragma once

#include <Model.h>
#include <WorldTransform.h>

enum class LRDirecition {

	kRight,
	kLeft,
};

class Player {

public:

	LRDirecition lrDirection_ = LRDirecition::kRight;
	static inline const float kAcceleration = 0.05f;
	static inline const float kLimitRunSpeed = 1.0f;
	static inline const float kAttenuation = 0.1f;
	static inline const float kTimeTurn = 0.3f;

	float turnFirstRotesionY_ = 0.0f;
	float turnTimer_ = 0.0f;

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
