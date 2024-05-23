#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {

	delete model_;
	delete debugCamera_;

}

GameScene::~GameScene() {

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {

			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

}

void GameScene::Initialize() {

	const uint32_t kNumBlockVirtical = 10;
	const uint32_t kNumBlockHorizontal = 20;

	const float kBlockHeight = 2.0f;
	const float kBlockWidth = 2.0f;

	worldTransformBlocks_.resize(kNumBlockVirtical);

	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {

		//worldTransformBlocks_[i] = new WorldTransform();
		//worldTransformBlocks_[i]->Initialize();
		//worldTransformBlocks_[i]->translation_.x = kBlockWidth * i;
		//worldTransformBlocks_[i]->translation_.y = 0.0f;

		worldTransformBlocks_[i].resize(kNumBlockHorizontal);

	}

	for (uint32_t i = 0; i < kNumBlockVirtical; i++) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {

			if ((i + j) % 2 == 0) {

				worldTransformBlocks_[i][j] = nullptr;
			} else {

				worldTransformBlocks_[i][j] = new WorldTransform();
				worldTransformBlocks_[i][j]->Initialize();
				worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
				worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
			}
		}
	}

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	model_ = Model::Create();

	worldTransform_.Initialize();
	viewProjection_.Initialize();

	debugCamera_ = new DebugCamera(1280, 720);
}

void GameScene::Update() {



	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {

				continue;
			}

			worldTransformBlock->scale_;
			worldTransformBlock->rotation_;
			worldTransformBlock->translation_;

			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			worldTransformBlock->TransferMatrix();
		}
	}

#ifdef _DEBUG

	if (input_->TriggerKey(DIK_SPACE)) {

		if (isDebugCameraActive_ == 1) {
			isDebugCameraActive_ = 0;
		} else {
			isDebugCameraActive_ = 1;
		}
	}

#endif // _DEBUG

	if (isDebugCameraActive_) {

		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;

		viewProjection_.TransferMatrix();
	} else {

		viewProjection_.UpdateMatrix();
	}

	//debugCamera_->Update();
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

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {

				continue;
			}

			model_->Draw(*worldTransformBlock, debugCamera_->GetViewProjection());
		}
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
