#include "Player.h"
#include "cassert"

void Player::Initialize(Model* model, uint32_t textureHandle) {

	assert(model);
	assert(textureHandle);

	worldTransform_.Initialize();
}

void Player::Update() {}

void Player::Draw() {}
