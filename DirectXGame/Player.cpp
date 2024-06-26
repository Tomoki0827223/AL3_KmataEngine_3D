#include "Player.h"
#include "Input.h"
#include "MathUtilityForText.h"
#include <algorithm>
#include <cassert>
#include <numbers>
#define NOMINMAX

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

		Vector3 acceleraiton = {};
		if (Input::GetInstance()->PushKey(DIK_RIGHT)) {

			if (velocity_.x < 0.0f) {
				velocity_.x *= (1.0f - kAttenuation);
			}
			if (lrDirection_ != LRDirecition::kRight) {
				lrDirection_ = LRDirecition::kRight;

				turnFirstRotesionY_ = worldTransform_.rotation_.y;
				turnTimer_ = kTimeTurn;
			}

			acceleraiton.x += kAcceleration;

		} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {

			if (velocity_.x > 0.0f) {
				velocity_.x *= (1.0f - kAttenuation);
			}
			if (lrDirection_ != LRDirecition::kLeft) {
				lrDirection_ = LRDirecition::kLeft;

				turnFirstRotesionY_ = worldTransform_.rotation_.y;
				turnTimer_ = kTimeTurn;
			}

			acceleraiton.x -= kAcceleration;
		}

		velocity_.x += acceleraiton.x;
		velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
	} else {

		velocity_.x *= (1.0f - kAttenuation);
	}

	if (turnTimer_ > 0.0f) {

		turnTimer_ -= 1.0f / 60.0f;

		float destinationRoteationYTable[] = {

		    std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};

		float destiaonRoteationY = destinationRoteationYTable[static_cast<uint32_t>(lrDirection_)];

		worldTransform_.rotation_.y = easeInOutSine(destiaonRoteationY, turnFirstRotesionY_, turnTimer_ / kTimeTurn);
	}

	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.UpdateMatrix();
}

void Player::Draw() { model_->Draw(worldTransform_, *viewProjection_, textureHandle_); }