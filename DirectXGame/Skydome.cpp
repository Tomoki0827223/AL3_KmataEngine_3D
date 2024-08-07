#include "Skydome.h"
#include <cassert>

void Skydome::Initialize(Model* model, ViewProjection* viewProjection) {
	assert(model);

	// 3Dモデルの作成
	Sky_Model_ = model;

	// ワールドトランスフォームの初期化
	Sky_worldTransform_.Initialize();

	Sky_viewProjection_ = viewProjection;
}
void Skydome::Update() { Sky_worldTransform_.TransferMatrix(); }
void Skydome::Draw() { Sky_Model_->Draw(Sky_worldTransform_, *Sky_viewProjection_); }