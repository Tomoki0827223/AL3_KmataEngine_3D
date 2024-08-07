#pragma once
#include "GameScene.h"
#include "AxisIndicator.h"
#include "TextureManager.h"
#include "affine.h"
#include <cassert>

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
	delete modelEnemy_;
	enemies_.clear();

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

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

	Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(18, 18);
	Enemy* newEnemy = new Enemy();
	newEnemy->Initialize(modelEnemy_, &viewProjection_, enemyPosition);
	enemies_.push_back(newEnemy);

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
	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

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