#include "Enemy.h"
#include "MathUtilityForText.h"
#include <cassert>
#include <numbers>

void Enemy::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) 
{ 
	assert(model);
	model_ = model;

	worldTranform_.Initialize();
	worldTranform_.translation_ = position;
	worldTranform_.rotation_.y = std::numbers::pi_v<float> * 3.0f / 2.0f;

	viewProjection_ = viewProjection;

	velocity_ = {-kWalkSpeed, 0, 0};
	walkTimer = 0.0f;
}

void Enemy::Update() 
{ 
	worldTranform_.translation_ += velocity_;
	walkTimer += 1.0f / 60.0f;
	//worldTranform_.rotation_.x = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer / kWalkMotionTime);

	float parm = std::sin(std::numbers::pi_v<float> * 2.0f * walkTimer / kWalkMotionTime);
	float radian = kWalkMotionAngleStart + kWalkMontionAngleEnd * (parm + 1.0f) / 2.0f;
	worldTranform_.rotation_.x = radian;

	worldTranform_.UpdateMatrix();

}

void Enemy::Draw() 
{

	model_->Draw(worldTranform_, *viewProjection_);

}

Vector3 Enemy::GetWorldPosition() const {
	Vector3 worldPos;
	// ワールド行列から平行移動成分を取り出す
	worldPos.x = worldTranform_.matWorld_.m[3][0];
	worldPos.y = worldTranform_.matWorld_.m[3][1];
	worldPos.z = worldTranform_.matWorld_.m[3][2];
	return worldPos;
}

AABB Enemy::GetAABB() const {
	Vector3 worldPos = GetWorldPosition();
	AABB aabb;
	aabb.min = {worldPos.x - radius_, worldPos.y - radius_, worldPos.z - radius_};
	aabb.max = {worldPos.x + radius_, worldPos.y + radius_, worldPos.z + radius_};
	return aabb;
}

void Enemy::OnCollision(const Player* player) {
	(void)player; // 現在は使用しない
}