#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "affine.h"
#include "AxisIndicator.h"

GameScene::GameScene() {

	delete model_;
	delete playerResorces_;
	delete debugCamera_;
	delete mapChipField_;
	delete player_;
	delete cameraController_;
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

	//AxisIndicator::GetInstance()->SetVisible(true);
	//AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	model_ = Model::Create();
	playerResorces_ = Model::CreateFromOBJ("player", true);

	worldTransform_.Initialize();
	viewProjection_.Initialize();

	debugCamera_ = new DebugCamera(1280, 720);

	mapChipField_ = new MapChipField;

	//mapChipField_->LoadMapChipCsv("Resources/blocks.csv");
	mapChipField_->LoadMapChipCsv("Resources/map.csv");


	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(18, 17);

	player_ = new Player();
	player_->Initialize(playerResorces_, &viewProjection_, playerPosition);

	GenerateBlocks();

	cameraController_ = new CameraController();
	cameraController_->Initialize();

}

void GameScene::GenerateBlocks() {

	uint32_t numBlockVisitacal = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	worldTransformBlocks_.resize(numBlockHorizontal);

	for (uint32_t i = 0; i < numBlockVisitacal; i++) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	for (uint32_t i = 0; i < numBlockVisitacal; i++) {
		for (uint32_t j = 0; j < numBlockHorizontal; j++) {
		
			if (mapChipField_->GetMapChipTypeByIndex(j,i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}

}

void GameScene::Update() {

	player_->Update();

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {

				continue;
			}

			worldTransformBlock->scale_;
			worldTransformBlock->rotation_;
			worldTransformBlock->translation_;

			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			worldTransformBlock->UpdateMatrix();
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

	// debugCamera_->Update();
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

	player_->Draw();

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
