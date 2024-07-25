#include "DethParticles.h"

void DethParticles::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	
	for (auto& worldTransform : worldTransforms_) {
		
		worldTransform.Initialize();
		worldTransform.translation_ = position;

	}


	for (auto& velocity : velocities_) {
		velocity = {0.1f, 0.1f, 0.1f};
	}

	viewProjection_ = viewProjection;
	
}

void DethParticles::Update() {
	for (size_t i = 0; i < kNumParticles; ++i) {
		// 移動
		worldTransforms_[i].translation_.x += velocities_[i].x;
		worldTransforms_[i].translation_.y += velocities_[i].y;
		worldTransforms_[i].translation_.z += velocities_[i].z;

		// 行列計算
		worldTransforms_[i].UpdateMatrix();
	}
}

void DethParticles::Draw() {

	// 3Dモデルを描画
	model_->Draw(worldTransforms_, *viewProjection_);
}