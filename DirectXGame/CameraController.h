#pragma once

#include "Vector3.h"
#include "ViewProjection.h"
#include "Player.h"

class Player {
public:
	Player();
	~Player();

private:
};

Player::Player() {}

Player::~Player() {}

class CameraController {
public:

	Player* taget_ = nullptr;

	void Initialize(ViewProjection* viewProjection, Vector3* target);
	void Update();

private:

	ViewProjection* viewProjection_ = nullptr;
	Vector3* target_ = nullptr;
	Vector3 offset_ = {0.0f, 5.0f, -10.0f}; // カメラのオフセット
};