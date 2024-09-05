#include "GameScene.h"
#include "AxisIndicator.h"
#include "TextureManager.h"
#include "affine.h"
#include <cassert>

GameScene::GameScene() {

}

GameScene::~GameScene() {
	// メモリの解放
	delete model_;
	delete playerResorces_;
	delete debugCamera_;
	
	delete mapChipField_;
	delete mapChipField_1;

	delete player_;
	delete cameraController_;
	delete enemy_;

	// ブロックのメモリ解放
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

		// ブロックのメモリ解放
	for (std::vector<WorldTransform*>& worldTransformBlockLine_1 : worldTransformBlocks_1) {
		for (WorldTransform* worldTransformBlock_1 : worldTransformBlockLine_1) {
			delete worldTransformBlock_1;
		}
	}
	worldTransformBlocks_1.clear();
}

void GameScene::Initialize() {
	// シングルトンインスタンスの取得
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// モデルの作成
	model_ = Model::CreateFromOBJ("block");
	//model_ = Model::Create();
	playerResorces_ = Model::CreateFromOBJ("player");
	modelEnemy_ = Model::CreateFromOBJ("enemy");

	// ワールドトランスフォームとビュー・プロジェクションの初期化
	worldTransform_.Initialize();
	viewProjection_.Initialize();

	// デバッグカメラの作成
	debugCamera_ = new DebugCamera(1280, 720);

	// マップチップフィールドの生成と初期化
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/Stage/map.csv");
	mapChipField_1 = new MapChipField;
	mapChipField_1->LoadMapChipCsv("Resources/Stage/map.csv");

	// プレイヤーの初期位置の取得
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(5, 17);
	Vector3 enmyPosition = mapChipField_->GetMapChipPositionByIndex(18, 18);

	// プレイヤーの生成と初期化
	player_ = new Player();
	player_->SetMapChipField(mapChipField_);
	player_->Initialize(playerResorces_, &viewProjection_, playerPosition);

	enemy_ = new Enemy();
	enemy_->Initialize(modelEnemy_, &viewProjection_, enmyPosition);

	// ブロックの生成
	GenerateBlocks();

	// カメラコントローラの生成と初期化
	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->setTarget(player_);

	// カメラの移動範囲を設定
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);
	cameraController_->Reset();
}

void GameScene::GenerateBlocks() {
	// マップチップフィールドからブロックの数を取得
	uint32_t numBlockVertical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	uint32_t numBlockVertical_1 = mapChipField_1->GetNumBlockVirtical();
	uint32_t numBlockHorizontal_1 = mapChipField_1->GetNumBlockHorizontal();

	// ワールドトランスフォームブロックのリサイズ
	worldTransformBlocks_.resize(numBlockVertical);

	worldTransformBlocks_1.resize(numBlockVertical_1);

	for (uint32_t i = 0; i < numBlockVertical; i++) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	for (uint32_t i = 0; i < numBlockVertical_1; i++) {
		worldTransformBlocks_[i].resize(numBlockHorizontal_1);
	}

	// マップチップフィールドの各セルに対してブロックを生成
	for (uint32_t i = 0; i < numBlockVertical; i++) {
		for (uint32_t j = 0; j < numBlockHorizontal; j++) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}

		// マップチップフィールドの各セルに対してブロックを生成
	for (uint32_t i = 0; i < numBlockVertical_1; i++) {
		for (uint32_t j = 0; j < numBlockHorizontal_1; j++) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform_1 = new WorldTransform();
				worldTransform_1->Initialize();
				worldTransformBlocks_1[i][j] = worldTransform_1;
				worldTransformBlocks_1[i][j]->translation_ = mapChipField_1->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::Update() {
	// ブロックのワールド行列を更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}

			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
			worldTransformBlock->UpdateMatrix();
		}
	}

	for (std::vector<WorldTransform*>& worldTransformBlockLine_1 : worldTransformBlocks_1) {
		for (WorldTransform* worldTransformBlock_1 : worldTransformBlockLine_1) {
			if (!worldTransformBlock_1) {
				continue;
			}

			worldTransformBlock_1->matWorld_ = MakeAffineMatrix(worldTransformBlock_1->scale_, worldTransformBlock_1->rotation_, worldTransformBlock_1->translation_);
			worldTransformBlock_1->UpdateMatrix();
		}
	}

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_SPACE)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif // _DEBUG

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
	} else {
		cameraController_->Update();
		viewProjection_.matView = cameraController_->GetViewProjection().matView;
		viewProjection_.matProjection = cameraController_->GetViewProjection().matProjection;
	}

	viewProjection_.TransferMatrix();
	player_->Update();
	enemy_->Update();
}

void GameScene::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);
	// ここに背景スプライトの描画処理を追加
	Sprite::PostDraw();

	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();

	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	// プレイヤーの描画
	player_->Draw();

	enemy_->Draw();

	// ブロックの描画
	for (const auto& worldTransformBlockLine : worldTransformBlocks_) {
		for (const auto& worldTransformBlock : worldTransformBlockLine) {
			if (worldTransformBlock) {
				model_->Draw(*worldTransformBlock, viewProjection_);
			}
		}
	}

	// ブロックの描画
	for (const auto& worldTransformBlockLine_1 : worldTransformBlocks_1) {
		for (const auto& worldTransformBlock_1 : worldTransformBlockLine_1) {
			if (worldTransformBlock_1) {
				model_->Draw(*worldTransformBlock_1, viewProjection_);
			}
		}
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();

	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);
	// ここに前景スプライトの描画処理を追加
	Sprite::PostDraw();
}
