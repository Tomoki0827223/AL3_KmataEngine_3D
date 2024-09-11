#include "CameraController.h"


void CameraController::Initialize(Model* model)
{ 

	model_ = model;
	worldTransform_.Initialize();
	viewProjection_.Initialize();
}

void CameraController::Update() {
	if (!target_) {
		return;
	}

	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	const Vector3& targetVelocity = target_->GetVelocity();

	// 追従対象とオフセットから目標座標を計算
	targetPosition_.x = targetWorldTransform.translation_.x + targetOffset_.x + targetVelocity.x * kVelocityBias;
	targetPosition_.y = targetWorldTransform.translation_.y + targetOffset_.y + targetVelocity.y * kVelocityBias;
	targetPosition_.z = targetWorldTransform.translation_.z + targetOffset_.z + targetVelocity.z * kVelocityBias;

	// 追従対象が画面外に出ないように補正
	targetPosition_.x = (std::max)(targetPosition_.x, targetWorldTransform.translation_.x - margin_.left);
	targetPosition_.x = (std::min)(targetPosition_.x, targetWorldTransform.translation_.x + margin_.right);
	targetPosition_.y = (std::max)(targetPosition_.y, targetWorldTransform.translation_.y - margin_.bottom);
	targetPosition_.y = (std::min)(targetPosition_.y, targetWorldTransform.translation_.y + margin_.top);

	// 座標補間によりゆったり追従
	viewProjection_.translation_ = Lerp(viewProjection_.translation_, targetPosition_, kInterpolationRate);

	// 移動範囲制限
	viewProjection_.translation_.x = (std::max)(viewProjection_.translation_.x, movableArea_.left);
	viewProjection_.translation_.x = (std::min)(viewProjection_.translation_.x, movableArea_.right);
	viewProjection_.translation_.y = (std::max)(viewProjection_.translation_.y, movableArea_.bottom);
	viewProjection_.translation_.y = (std::min)(viewProjection_.translation_.y, movableArea_.top);

	// 行列を更新する
	viewProjection_.UpdateMatrix();


	number1 = scoretotal;

	number2[0] = number1 / 10000;
	number1 %= 10000;
	fontposX[0] = 20;

	number2[1] = number1 / 1000;
	number1 %= 1000;
	fontposX[1] = fontposX[0] + 50;

	number2[2] = number1 / 100;
	number1 %= 100;
	fontposX[2] = fontposX[1] + 50;

	number2[3] = number1 / 10;
	number1 %= 10;
	fontposX[3] = fontposX[2] + 50;

	number2[4] = number1;
	fontposX[4] = fontposX[3] + 50;
}

void CameraController::Reset() {
	if (!target_) {
		return;
	}

	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	viewProjection_.translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	viewProjection_.translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	viewProjection_.translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;

	viewProjection_.UpdateMatrix();
}

void CameraController::Draw() {
	for (int i = 0; i < 5; ++i) {

		// モデルの描画
		model_->Draw(worldTransform_, viewProjection_);
	}
}

void CameraController::SetPosition(const Vector3& position) 
{ fontPosition_ = position; }
