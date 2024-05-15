#include "Player.h"
#include "cassert"

void Player::Initialize(Model* model, uint32_t textureHandle,ViewProjection* viewProjection) {

	assert(model);

	worldTransform_.Initialize();

	model_ = model;
	textureHandle_ = textureHandle;
	viewProjection_ = viewProjection;
}

void Player::Update() {

	worldTransform_.TransferMatrix();

}

void Player::Draw() {

	model_->Draw(worldTransform_, *viewProjection_, textureHandle_);


}
