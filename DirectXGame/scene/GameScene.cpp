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
	delete deathParticles_;
	delete modelParticles_;

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();

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

	skydome_ = new Skydome();

	// モデルの作成
	model_ = Model::CreateFromOBJ("block");
	// model_ = Model::Create();
	playerResorces_ = Model::CreateFromOBJ("player");
	modelEnemy_ = Model::CreateFromOBJ("enemy");
	modelParticles_ = Model::CreateFromOBJ("deathParticle");
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

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
	Vector3 enmyPosition = mapChipField_->GetMapChipPositionByIndex(18, 18);

	// プレイヤーの生成と初期化
	player_ = new Player();
	player_->SetMapChipField(mapChipField_);
	player_->Initialize(playerResorces_, &viewProjection_, playerPosition);

	// パーティクル生成
	deathParticles_ = new DeathParticles;
	deathParticles_->Initialize(modelParticles_, &viewProjection_, playerPosition);

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

	phace_ = Phase::kPlay;
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

//void GameScene::Update() {
//	// ブロックのワールド行列を更新
//	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
//		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
//			if (!worldTransformBlock) {
//				continue;
//			}
//
//			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
//			worldTransformBlock->UpdateMatrix();
//		}
//	}
//
//#ifdef _DEBUG
//	if (input_->TriggerKey(DIK_SPACE)) {
//		isDebugCameraActive_ = !isDebugCameraActive_;
//	}
//#endif // _DEBUG
//
//	if (isDebugCameraActive_) {
//		debugCamera_->Update();
//		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
//		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
//	} else {
//		cameraController_->Update();
//		viewProjection_.matView = cameraController_->GetViewProjection().matView;
//		viewProjection_.matProjection = cameraController_->GetViewProjection().matProjection;
//	}
//
//	viewProjection_.TransferMatrix();
//	player_->Update();
//
//	for (Enemy* enemy : enemies_) {
//		enemy->Update();
//	}
//
//	CheckAllCollisions();
//}

void GameScene::Update() {

	ChangePhase();

	switch (phace_) {
	case Phase::kPlay:

		// 自キャラの更新
		player_->Update();

		// 天球の更新
		skydome_->Update();

		// カメラコントローラの更新
		cameraController_->Update();

		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

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

		// カメラ処理
		if (isDebugCameraActive_) {
			// デバッグカメラの更新
			debugCamera_->Update();
			viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の転送
			viewProjection_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
			viewProjection_.matView = cameraController_->GetViewProjection().matView;
			viewProjection_.matProjection = cameraController_->GetViewProjection().matProjection;
			// ビュープロジェクションの転送
			viewProjection_.TransferMatrix();
		}

#ifdef _DEBUG
		if (input_->TriggerKey(DIK_SPACE)) {
			if (isDebugCameraActive_ == true)
				isDebugCameraActive_ = false;
			else
				isDebugCameraActive_ = true;
		}
#endif

		// 全てのあたり判定を行う
		CheckAllCollisions();

		break;
	case Phase::kDeath:

		// 天球の更新
		skydome_->Update();

		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

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

		// カメラ処理
		if (isDebugCameraActive_) {
			// デバッグカメラの更新
			debugCamera_->Update();
			viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の転送
			viewProjection_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
			viewProjection_.matView = cameraController_->GetViewProjection().matView;
			viewProjection_.matProjection = cameraController_->GetViewProjection().matProjection;
			// ビュープロジェクションの転送
			viewProjection_.TransferMatrix();
		}

		if (deathParticles_) {
			deathParticles_->Update();
		}

		break;
	}
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

	//// プレイヤーの描画
	//player_->Draw();

	switch (phace_) {
	case GameScene::Phase::kPlay:

		skydome_->Draw();

		// 自キャラの描画
		player_->Draw();

		for (Enemy* enemy : enemies_) {
			enemy->Draw();
		}

		if (!player_->IsDead()) {
			player_->Draw();
		}


		// ブロックの描画
		for (const auto& worldTransformBlockLine : worldTransformBlocks_) {
			for (const auto& worldTransformBlock : worldTransformBlockLine) {
				if (worldTransformBlock) {
					model_->Draw(*worldTransformBlock, viewProjection_);
				}
			}
		}

		break;
	case GameScene::Phase::kDeath:

		if (deathParticles_) {
			deathParticles_->Draw();
		}

		break;
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();

	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);
	// ここに前景スプライトの描画処理を追加
	Sprite::PostDraw();
}

void GameScene::CheckAllCollisions() {

#pragma region 自キャラと敵キャラの当たり判定

	// 判定対象1と2の座標
	AABB aabb1, aabb2;

	// 自キャラの座標
	aabb1 = player_->GetAABB();
	for (Enemy* enemy : enemies_) {
		// 敵弾の座標
		aabb2 = enemy->GetAABB();

		// AABB同士の交差判定(
		if (IsCollisionAABB(aabb1, aabb2)) {
			// 自キャラの衝突判定コールバックを呼び出す
			player_->OnCollision(enemy);
			// 敵弾の衝突判定コールバックを呼び出す
			enemy->OnCollision(player_);
		}
	}
#pragma endregion
}

void GameScene::ChangePhase() {

	switch (phace_) {
	case Phase::kPlay:

		// 自キャラの状態をチェック
		if (player_->IsDead()) {
			// 死亡フェーズに切り替え
			phace_ = Phase::kDeath;
			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();
			// 自キャラの座標にデスパーティクルを発生、初期化
			deathParticles_ = new DeathParticles();
			deathParticles_->Initialize(modelParticles_, &viewProjection_, deathParticlesPosition);
		}

		// 全てのあたり判定を行う
		CheckAllCollisions();

		
		break;
	case Phase::kDeath:
		
		// デス演出フェーズの処理
		if (deathParticles_ && deathParticles_->IsFinished()) {
			finished_ = true;
		}

		break;
	}
}