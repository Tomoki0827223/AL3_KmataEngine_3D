#pragma once
#include "GameScene.h"

GameScene::GameScene() {}

GameScene::~GameScene() {

	delete debugCamera_;

	delete skydome;
	delete mapChipField_;
	delete modelPlayer_;
	delete modelBlock_;
	delete modelSkydome_;
	delete cameraController_;

	delete dethParticles_;
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}

	delete player_;
	delete modelEnemy_;


	//for (int32_t i = 0; i < 3; i++) {
	//	delete newEnemy;
	//}

	enemies_.clear();

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	// numBlockVirtical と numBlockHorizontal は、配列のサイズです。
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			// ポインタがnullptrでないことを確認してからdeleteする
			if (worldTransformBlocks_[i][j] != nullptr) {
				delete worldTransformBlocks_[i][j];    // メモリを解放する
				worldTransformBlocks_[i][j] = nullptr; // ポインタをnullptrに設定して二重解放を防ぐ
			}
		}
	}

}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	worldTransform_.Initialize();
	viewProjection_.Initialize();

	// SkyDome作成
	skydome = new Skydome;
	// Mapの生成
	mapChipField_ = new MapChipField;
	// 自キャラの生成
	player_ = new Player;

	deathParticles_ = new DeathParticles;

	// Mapのよみこみ
	mapChipField_->LoadMapChipCsv("Resources/Stage/map.csv");
	modelPlayer_ = Model::CreateFromOBJ("player", true);
	modelEnemy_ = Model::CreateFromOBJ("enemy");
	modelBlock_ = Model::CreateFromOBJ("block");
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	modelParticles_ = Model::CreateFromOBJ("deathParticle", true);

	// プレイヤーの初期位置の取得
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(5, 18);

	// 敵の初期位置の取得
	Vector3 enmyPosition = mapChipField_->GetMapChipPositionByIndex(20, 18);

	skydome->Initialize(modelSkydome_, &viewProjection_);
	// プレイヤーの生成と初期化
	player_->SetMapChipField(mapChipField_);
	player_->Initialize(modelPlayer_, &viewProjection_, playerPosition);

	// パーティクル生成
	dethParticles_ = new DeathParticles;
	dethParticles_->Initialize(modelParticles_, &viewProjection_, playerPosition);

	for (int32_t i = 0; i < 3; i++) {

		Enemy* newEnemy = new Enemy();
		//Enemy* newEnemy1 = new Enemy();
		//Enemy* newEnemy2 = new Enemy();
		//Enemy* newEnemy3 = new Enemy();
		//Enemy* newEnemy4 = new Enemy();
		//Enemy* newEnemy5 = new Enemy();
		//Enemy* newEnemy6 = new Enemy();

		//Enemy* newEnemy7 = new Enemy();
		//Enemy* newEnemy8 = new Enemy();
		//Enemy* newEnemy9 = new Enemy();

		//Enemy* newEnemy10 = new Enemy();
		//Enemy* newEnemy11 = new Enemy();
		//Enemy* newEnemy12 = new Enemy();
		//Enemy* newEnemy13 = new Enemy();
		//Enemy* newEnemy14 = new Enemy();
		//Enemy* newEnemy15 = new Enemy();
		//Enemy* newEnemy16 = new Enemy();
		//Enemy* newEnemy17 = new Enemy();
		//Enemy* newEnemy18 = new Enemy();
		//Enemy* newEnemy19 = new Enemy();

		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(18, 18);
		//Vector3 enemyPosition1 = mapChipField_->GetMapChipPositionByIndex(18, 17);
		//Vector3 enemyPosition2 = mapChipField_->GetMapChipPositionByIndex(18, 16);
		//Vector3 enemyPosition3 = mapChipField_->GetMapChipPositionByIndex(18, 15);
		//Vector3 enemyPosition4 = mapChipField_->GetMapChipPositionByIndex(18, 14);
		//Vector3 enemyPosition5 = mapChipField_->GetMapChipPositionByIndex(18, 13);
		//Vector3 enemyPosition6 = mapChipField_->GetMapChipPositionByIndex(18, 12);

		//Vector3 enemyPosition7 = mapChipField_->GetMapChipPositionByIndex(25, 16);
		//Vector3 enemyPosition8 = mapChipField_->GetMapChipPositionByIndex(25, 17);
		//Vector3 enemyPosition9 = mapChipField_->GetMapChipPositionByIndex(25, 18);

		//Vector3 enemyPosition10 = mapChipField_->GetMapChipPositionByIndex(35, 21);
		//Vector3 enemyPosition11 = mapChipField_->GetMapChipPositionByIndex(35, 20);
		//Vector3 enemyPosition12 = mapChipField_->GetMapChipPositionByIndex(35, 19);
		//Vector3 enemyPosition13 = mapChipField_->GetMapChipPositionByIndex(35, 18);
		//Vector3 enemyPosition14 = mapChipField_->GetMapChipPositionByIndex(35, 17);
		//Vector3 enemyPosition15 = mapChipField_->GetMapChipPositionByIndex(35, 16);
		//Vector3 enemyPosition16 = mapChipField_->GetMapChipPositionByIndex(35, 15);
		//Vector3 enemyPosition17 = mapChipField_->GetMapChipPositionByIndex(35, 14);
		//Vector3 enemyPosition18 = mapChipField_->GetMapChipPositionByIndex(35, 13);
		//Vector3 enemyPosition19 = mapChipField_->GetMapChipPositionByIndex(35, 12);

		newEnemy->Initialize(modelEnemy_, &viewProjection_, enemyPosition);
		//newEnemy1->Initialize(modelEnemy_, &viewProjection_, enemyPosition1);
		//newEnemy2->Initialize(modelEnemy_, &viewProjection_, enemyPosition2);
		//newEnemy3->Initialize(modelEnemy_, &viewProjection_, enemyPosition3);
		//newEnemy4->Initialize(modelEnemy_, &viewProjection_, enemyPosition4);
		//newEnemy5->Initialize(modelEnemy_, &viewProjection_, enemyPosition5);
		//newEnemy6->Initialize(modelEnemy_, &viewProjection_, enemyPosition6);

		//newEnemy7->Initialize(modelEnemy_, &viewProjection_, enemyPosition7);
		//newEnemy8->Initialize(modelEnemy_, &viewProjection_, enemyPosition8);
		//newEnemy9->Initialize(modelEnemy_, &viewProjection_, enemyPosition9);

		//newEnemy10->Initialize(modelEnemy_, &viewProjection_, enemyPosition10);
		//newEnemy11->Initialize(modelEnemy_, &viewProjection_, enemyPosition11);
		//newEnemy12->Initialize(modelEnemy_, &viewProjection_, enemyPosition12);
		//newEnemy13->Initialize(modelEnemy_, &viewProjection_, enemyPosition13);
		//newEnemy14->Initialize(modelEnemy_, &viewProjection_, enemyPosition14);
		//newEnemy15->Initialize(modelEnemy_, &viewProjection_, enemyPosition15);
		//newEnemy16->Initialize(modelEnemy_, &viewProjection_, enemyPosition16);
		//newEnemy17->Initialize(modelEnemy_, &viewProjection_, enemyPosition17);
		//newEnemy18->Initialize(modelEnemy_, &viewProjection_, enemyPosition18);
		//newEnemy19->Initialize(modelEnemy_, &viewProjection_, enemyPosition19);

		enemies_.push_back(newEnemy);
		//enemies_.push_back(newEnemy1);
		//enemies_.push_back(newEnemy2);
		//enemies_.push_back(newEnemy3);
		//enemies_.push_back(newEnemy4);
		//enemies_.push_back(newEnemy5);
		//enemies_.push_back(newEnemy6);

		//enemies_.push_back(newEnemy7);
		//enemies_.push_back(newEnemy8);
		//enemies_.push_back(newEnemy9);

		//enemies_.push_back(newEnemy10);
		//enemies_.push_back(newEnemy11);
		//enemies_.push_back(newEnemy12);
		//enemies_.push_back(newEnemy13);
		//enemies_.push_back(newEnemy14);
		//enemies_.push_back(newEnemy15);
		//enemies_.push_back(newEnemy16);
		//enemies_.push_back(newEnemy17);
		//enemies_.push_back(newEnemy18);
		//enemies_.push_back(newEnemy19);
	}

	// パーティクル生成
	dethParticles_ = new DeathParticles;
	dethParticles_->Initialize(modelParticles_, &viewProjection_, playerPosition);

	debugCamera_ = new DebugCamera(1280, 720);

	GenerateBlocks();

	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->setTarget(player_);
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);
	cameraController_->Reset();

	phase_ = Phase::kPlay;
}

void GameScene::GenerateBlocks() {
	//// 要素数
	//uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	//uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	// 列数を設定(縦方向のブロック数)
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		// 1列の要素数を指定(横方向のブロック数)
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {

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

	switch (phase_) {
	case Phase::kPlay:
		// ゲームプレイフェーズの処理
		// 自キャラの更新
		player_->Update();
		if (player_->IsDead()) {
			ChangePhase();
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;
			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();
			// 自キャラの座標にデスパーティクルを発生、初期化
			deathParticles_->Initialize(modelParticles_, &viewProjection_, deathParticlesPosition);
		}

		skydome->Update();

		cameraController_->Update();

		viewProjection_.TransferMatrix();

		CheckAllCollisions();

		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

#ifdef _DEBUG

		if (input_->TriggerKey(DIK_SPACE)) {
			isDebugCameraActive_ = !isDebugCameraActive_;
		}
#endif

		if (isDebugCameraActive_) {
			debugCamera_->Update();
			viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;

		} else {
			viewProjection_.matView = cameraController_->GetViewProjection().matView;
			viewProjection_.matProjection = cameraController_->GetViewProjection().matProjection;
		}

		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;

				worldTransformBlock->UpdateMatrix();
				worldTransformBlock->TransferMatrix();
			}
		}

		break;
	case Phase::kDeath:
		// デス演出フェースの処理

#ifdef _DEBUG

		if (input_->TriggerKey(DIK_SPACE)) {
			isDebugCameraActive_ = !isDebugCameraActive_;
		}
#endif

		if (isDebugCameraActive_) {
			debugCamera_->Update();
			viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;

		} else {
			viewProjection_.matView = cameraController_->GetViewProjection().matView;
			viewProjection_.matProjection = cameraController_->GetViewProjection().matProjection;
		}

		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;

				worldTransformBlock->UpdateMatrix();
				worldTransformBlock->TransferMatrix();
			}
		}

		skydome->Update();

		cameraController_->Update();

		viewProjection_.TransferMatrix();

		CheckAllCollisions();

		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		if (dethParticles_) {
			dethParticles_->Update();
		}

		ChangePhase();

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
	///

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
	///

	switch (phase_) {
	case Phase::kPlay:;

		// 自キャラ
		player_->Draw();
		skydome->Draw();

		for (Enemy* enemy : enemies_) {
			enemy->Draw();
		}

		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				modelBlock_->Draw(*worldTransformBlock, viewProjection_);
			}
		}
		break;
	case Phase::kDeath:

		skydome->Draw();
		dethParticles_->Draw();

		for (Enemy* enemy : enemies_) {
			enemy->Draw();
		}

		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;
				modelBlock_->Draw(*worldTransformBlock, viewProjection_);
			}
		}

		break;
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();

	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);
	Sprite::PostDraw();

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
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

	switch (phase_) {
	case Phase::kPlay:

		// 自キャラの状態をチェック
		if (player_->IsDead()) {
			// 死亡フェーズに切り替え
			phase_ = Phase::kDeath;
			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();
			// 自キャラの座標にデスパーティクルを発生、初期化
			dethParticles_ = new DeathParticles();
			dethParticles_->Initialize(modelParticles_, &viewProjection_, deathParticlesPosition);
		}

		// 全てのあたり判定を行う






		CheckAllCollisions();

		break;
	case Phase::kDeath:

		// デス演出フェーズの処理
		if (dethParticles_ && dethParticles_->IsFinished()) {
			finished_ = true;
		}

		break;
	}
}