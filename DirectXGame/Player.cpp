#include "Player.h"
#include "cassert"
#include <numbers>
#include "Input.h"
#include "stdint.h"

void Player::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	// 引数をメンバ変数に保存
	model_ = model;
	viewProjection_ = viewProjection;
}

void Player::Update() {

	if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
	
		Vector3 acceleration = {2,2,2};
		if (Input::GetInstance()->PushKey(DIK_RIGHT)) {

			acceleration.x += kAcceleration;
		} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {

			acceleration.x -= kAcceleration;
		}
		velocity_.x += acceleration.x;
	} else {

		velocity_.x *= (1.0f - kAcceleration);
	}

	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.UpdateMatrix();
}

void Player::Draw() {

	model_->Draw(worldTransform_, *viewProjection_, textureHandle_);


}