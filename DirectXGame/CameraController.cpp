#include "CameraController.h"

void CameraController::Initialize(ViewProjection* viewProjection, Player* target) {
	viewProjection_ = viewProjection;
	target_ = target;
}

void CameraController::Update() {

	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	
	viewProjection_->translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	
	viewProjection_->UpdateMatrix();
}

void CameraController::Reset() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	
	viewProjection_->translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
}
