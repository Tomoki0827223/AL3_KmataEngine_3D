#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "affine.h"

GameScene::GameScene() { delete mapChipField_; }

GameScene::~GameScene() {


	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	worldTransformBlocks_.clear();

}

void GameScene::Initialize() {

	//	// 要素数
	//const uint32_t kNumBlockVirtical = 10;
	//const uint32_t kNumBlockHorizontal = 20;
	//// ブロック1個分の横幅
	//const float kBlockWidth = 2.0f;
	//const float kBlockHeight = 2.0f;
	//// 要素数を変更する
	//worldTransformBlocks_.resize(kNumBlockVirtical);

	//// キューブの生成
	//for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
	//	worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	//}

	//for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
	//	for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
	//		if (j % 2 == (i % 2)) {
	//			worldTransformBlocks_[i][j] = new WorldTransform();
	//			worldTransformBlocks_[i][j]->Initialize();
	//			worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
	//			worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
	//		} else {
	//			worldTransformBlocks_[i][j] = nullptr;
	//		}
	//	}
	//}



	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/map.csv");

}

void GameScene::GenerateBlocks() {

	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// ブロック1個分の横幅
	//const float kBlockWidth = 2.0f;
	//const float kBlockHeight = 2.0f;
	
	// 要素数を変更する
	worldTransformBlocks_.resize(20);

	// キューブの生成
	for (uint32_t i = 0; i < 20; ++i) {
		worldTransformBlocks_[i].resize(100);
	}

	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform;
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			} else {
				/*worldTransformBlocks_[i][j] = nullptr;*/
			}
		}
	}

}

void GameScene::Update() {

	// 縦横ブロック更新
	for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko)
				continue;

			// アフィン変換行列の作成
			//(MakeAffineMatrix：自分で作った数学系関数)
			worldTransformBlockYoko->matWorld_ = MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

			// 定数バッファに転送
			worldTransformBlockYoko->TransferMatrix();

		}
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
