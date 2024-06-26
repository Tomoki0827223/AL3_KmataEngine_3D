#pragma once

#include "Vector3.h"
#include "ViewProjection.h"
#include "Player.h"

class Player;

class CameraController {
public:

	Player* taget_ = nullptr;

	void Initialize(ViewProjection* viewProjection, Vector3* target);
	void Update();

	void setTaget(Player* taget) { target_ = target_; }
	void Reset();

	Vector3 targetOffset_ = {0, 0, -15.0f};

private:

	ViewProjection* viewProjection_ = nullptr;
	Vector3* target_ = nullptr;
	Vector3 offset_ = {0.0f, 5.0f, -10.0f}; // カメラのオフセット
};