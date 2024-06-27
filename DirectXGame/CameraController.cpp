#include "CameraController.h"
#include "Player.h"

void CameraController::Initialize() { viewProjection_.Initialize(); }

void CameraController::Update() {

	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	const Vector3& targetvelocity_ = target_->GetVelocity();
	const Vector3& targePosition = target_->GetPlayerPosition();

	arriLevel_point_.x = targetWorldTransform.translation_.x + targetOffset_.x + targetvelocity_.x + kVelocityBitz;
	arriLevel_point_.y = targetWorldTransform.translation_.y + targetOffset_.y + targetvelocity_.y + kVelocityBitz;
	arriLevel_point_.z = targetWorldTransform.translation_.z + targetOffset_.z + targetvelocity_.z + kVelocityBitz;

	viewProjection_.translation_ = Lerp(viewProjection_.translation_, arriLevel_point_, kInterpolatiomrate);

	viewProjection_.translation_.x = (std::max)(viewProjection_.translation_.x, targePosition.x, +margin.left);
	viewProjection_.translation_.x = (std::min)(viewProjection_.translation_.x, targePosition.x, +margin.Rihgth);
	viewProjection_.translation_.y = (std::max)(viewProjection_.translation_.y, targePosition.y, +margin.bottom);
	viewProjection_.translation_.y = (std::min)(viewProjection_.translation_.y, targePosition.y, +margin.top);

	viewProjection_.translation_.x = (std::max)(viewProjection_.translation_.x, movebleArea_.left);
	viewProjection_.translation_.x = (std::min)(viewProjection_.translation_.x, movebleArea_.Rihgth);
	viewProjection_.translation_.y = (std::max)(viewProjection_.translation_.y, movebleArea_.bottom);
	viewProjection_.translation_.y = (std::min)(viewProjection_.translation_.y, movebleArea_.top);

	viewProjection_.UpdateMatrix();
}

void CameraController::Reset() {

	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();

	viewProjection_.translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	viewProjection_.translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	viewProjection_.translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;
}
