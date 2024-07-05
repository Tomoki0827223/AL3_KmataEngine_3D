#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <numbers>

class TurnController {
public:
	TurnController() : currentRotationY_(0.0f), turnFirstRotationY_(0.0f), turnTimer_(0.0f), turnDuration_(1.0f) {}

	void Initialize(float initialRotationY, float turnDuration);
	void StartTurn(float destinationRotationY);
	void UpdateTurn(float deltaTime);
	float GetCurrentRotationY();

private:
	float currentRotationY_;
	float turnFirstRotationY_;
	float destinationRotationY_;
	float turnTimer_;
	float turnDuration_;
};
