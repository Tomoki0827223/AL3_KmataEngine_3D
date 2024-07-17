#include "GameScene.h"
#include "AxisIndicator.h"
#include "TextureManager.h"
#include "affine.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	// メモリの解放
	delete model_;
	delete playerResorces_;
	delete debugCamera_;
	delete mapChipField_;
	delete player_;
	delete cameraController_;

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}

	// ブロックのメモリ解放
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

void GameScene::Initialize() {
	// シングルトンインスタンスの取得
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// モデルの作成
	model_ = Model::CreateFromOBJ("block");
	// model_ = Model::Create();
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

	// プレイヤーの初期位置の取得
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(5, 17);
	// Vector3 enmyPosition = mapChipField_->GetMapChipPositionByIndex(18, 18);

	// プレイヤーの生成と初期化
	player_ = new Player();
	player_->SetMapChipField(mapChipField_);
	player_->Initialize(playerResorces_, &viewProjection_, playerPosition);

	for (int32_t i = 0; i < 3; i++) {
		Enemy* newEnemy = new Enemy();
		Enemy* newEnemy1 = new Enemy();
		Enemy* newEnemy2 = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(18, 18);
		Vector3 enemyPosition1 = mapChipField_->GetMapChipPositionByIndex(18, 17);
		Vector3 enemyPosition2 = mapChipField_->GetMapChipPositionByIndex(18, 16);
		newEnemy->Initialize(modelEnemy_, &viewProjection_, enemyPosition);
		newEnemy1->Initialize(modelEnemy_, &viewProjection_, enemyPosition1);
		newEnemy2->Initialize(modelEnemy_, &viewProjection_, enemyPosition2);

		enemies_.push_back(newEnemy);
		enemies_.push_back(newEnemy1);
		enemies_.push_back(newEnemy2);
	}

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

	// ワールドトランスフォームブロックのリサイズ
	worldTransformBlocks_.resize(numBlockVertical);

	for (uint32_t i = 0; i < numBlockVertical; i++) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
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

	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}

	CheckAllCollisions();
}

void GameScene::CheckAllCollisions() {
#pragma region Player-Enemy Collisions
	AABB playerAABB = player_->GetAABB();
	for (Enemy* enemy : enemies_) {
		AABB enemyAABB = enemy->GetAABB();
		if (playerAABB.min.x <= enemyAABB.max.x && playerAABB.max.x >= enemyAABB.min.x && playerAABB.min.y <= enemyAABB.max.y && playerAABB.max.y >= enemyAABB.min.y &&
		    playerAABB.min.z <= enemyAABB.max.z && playerAABB.max.z >= enemyAABB.min.z) {
			// 衝突している場合の処理
			player_->OnCollision(enemy);
			enemy->OnCollision(player_);
		}
	}
#pragma endregion
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

	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	// ブロックの描画
	for (const auto& worldTransformBlockLine : worldTransformBlocks_) {
		for (const auto& worldTransformBlock : worldTransformBlockLine) {
			if (worldTransformBlock) {
				model_->Draw(*worldTransformBlock, viewProjection_);
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
