#pragma once

#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class Enemy {

public:

	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	void Update();

	void Draw();

private:

	static inline const float kWalkSpeed = 0.02f;
	static inline const float kWalkMotionAngleStart = 0.0f;
	static inline const float kWalkMontionAngleEnd = 30.0f;
	static inline const float kWalkMotionTime = 1.0f;

	Model* model_ = nullptr;
	WorldTransform worldTranform_;
	ViewProjection* viewProjection_ = nullptr;
	Vector3 velocity_ = {};
	float walkTimer = 0.0f;





};
