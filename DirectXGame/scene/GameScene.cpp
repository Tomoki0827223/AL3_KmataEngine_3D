#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {

	delete model_;

	for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {

		delete worldTransformBlock;
	}
	worldTransformBlocks_.clear();

}

void GameScene::Initialize() {

	const uint32_t kNumBlockHorizontal = 20;

	const float kBlockWidth = 2.0f;

	worldTransformBlocks_.resize(kNumBlockHorizontal);

	for (uint32_t i = 0; i < kNumBlockHorizontal; i++) {

		worldTransformBlocks_[i] = new WorldTransform();
		worldTransformBlocks_[i]->Initialize();
		worldTransformBlocks_[i]->translation_.x = kBlockWidth * i;
		worldTransformBlocks_[i]->translation_.y = 0.0f;

	}

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	model_ = Model::Create();

	worldTransform_.Initialize();
	viewProjection_.Initialize();
}

void GameScene::Update() {

	for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {

		worldTransformBlock->scale_;
		worldTransformBlock->rotation_;
		worldTransformBlock->translation_;

		worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

		worldTransformBlock->TransferMatrix();
	}


}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	for (WorldTransform* worldTransformBlock : worldTransformBlocks_) {

		model_->Draw(*worldTransformBlock, viewProjection_);
	
	}

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
