#pragma once

#include "Model.h"
#include "AABB.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Player.h"

class Player;

class Enemy {

public:

	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	void Update();

	void Draw();


	// AABBを取得する関数
	AABB GetAABB();
	// 衝突応答
	void OnCollision(const Player* player);

private:

	Vector3 worldPos_;
	float radius_;

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
