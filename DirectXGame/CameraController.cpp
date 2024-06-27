#include "CameraController.h"
#include "Player.h"

void CameraController::Initialize(ViewProjection* viewProjection, Player* target) {

	viewProjection_.Initialize();

}

void CameraController::Update() {

	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	viewProjection_.translation_ = targetWorldTransform.translation_ + targetOffset_;
	viewProjection_.UpdateMatrix();

}

void CameraController::Reset() {

	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	viewProjection_.translation_ = targetWorldTransform.translation_ + targetOffset_;

}
