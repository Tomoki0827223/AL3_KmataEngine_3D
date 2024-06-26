#include "CameraController.h"

void CameraController::Initialize(ViewProjection* viewProjection, Vector3* target) {
	viewProjection_ = viewProjection;
	target_ = target;
}

void CameraController::Update() {
	if (viewProjection_ && target_) {
		viewProjection_->eye = *target_ + offset_;
		viewProjection_->UpdateMatrix();
	}
}