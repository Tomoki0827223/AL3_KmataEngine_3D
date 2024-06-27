#include "CameraController.h"

void CameraController::Initialize() { viewProjection_->Initialize(); }

void CameraController::Update() {

	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	
	viewProjection_->translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	
	viewProjection_->UpdateMatrix();
}

void CameraController::Reset() {
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	
	viewProjection_->translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
}
