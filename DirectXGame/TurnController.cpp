#include "TurnController.h"

void TurnController::Initialize(float initialRotationY, float turnDuration) {
	currentRotationY_ = initialRotationY;
	turnDuration_ = turnDuration;
	turnFirstRotationY_ = initialRotationY;
	turnTimer_ = 0.0f;
}

void TurnController::StartTurn(float destinationRotationY) {
	turnFirstRotationY_ = currentRotationY_;
	destinationRotationY_ = destinationRotationY;
	turnTimer_ = 0.0f;
}

void TurnController::UpdateTurn(float deltaTime) {
	if (turnTimer_ < turnDuration_) {
		turnTimer_ += deltaTime;
		float t = std::clamp(turnTimer_ / turnDuration_, 0.0f, 1.0f);
		currentRotationY_ = turnFirstRotationY_ + (destinationRotationY_ - turnFirstRotationY_) * t;
	}
}

float TurnController::GetCurrentRotationY() { return currentRotationY_; }
