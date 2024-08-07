#include "DethParticles.h"

void DeathParticles::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {

	model_ = model;
	viewProjection_ = viewProjection;

	objectColor_.Initialize();
	color_ = {1, 1, 1, 1};

	for (auto& worldTransform : worldTransform_) {
		worldTransform.Initialize();
		worldTransform.translation_ = position;
	}
}

void DeathParticles::Update() {

	if (isFinished_) {
		return;
	}
	// パーティクル消える時間
	counter_ += 1.0f / 60.0f;
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		isFinished_ = true;
	}

	for (uint32_t i = 0; i < 8; ++i) {
		Vector3 velocity = {kSpeed, 0.0f, 0.0f};
		// 回転角を計算する
		float angle = kAngleUint * i;
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);
		velocity = Transform(velocity, matrixRotation);
		// 移動処理
		worldTransform_[i].translation_ += velocity;
	}

	color_.w = max(0.0f, 1.0f - counter_ / kDuration);
	objectColor_.SetColor(color_);
	objectColor_.TransferMatrix();

	// ワールド変換の更新
	for (auto& worldTransform : worldTransform_) {
		worldTransform.UpdateMatrix();
	}
}

void DeathParticles::Draw() {

	if (isFinished_) {
		return;
	}

	// モデルの描画
	for (const auto& worldTransform : worldTransform_) {
		model_->Draw(worldTransform, *viewProjection_, &objectColor_);
	}
}