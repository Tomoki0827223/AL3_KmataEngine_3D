#include "GameScene.h"

GameScene::GameScene() {

}

GameScene::~GameScene() {
	// メモリの解放
	delete model_;
	delete playerResorces_;
	delete debugCamera_;
	delete mapChipField_;
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

	bitMapFontTexture_ = Model::CreateFromOBJ("ZERO",true);
	//bitMapFontTexture_1 = TextureManager::Load("cunt/1.png");
	//bitMapFontTexture_2 = TextureManager::Load("cunt/2.png");
	//bitMapFontTexture_3 = TextureManager::Load("cunt/3.png");
	//bitMapFontTexture_4 = TextureManager::Load("cunt/4.png");
	//bitMapFontTexture_5 = TextureManager::Load("cunt/5.png");
	//bitMapFontTexture_6 = TextureManager::Load("cunt/6.png");
	//bitMapFontTexture_7 = TextureManager::Load("cunt/7.png");
	//bitMapFontTexture_8 = TextureManager::Load("cunt/8.png");
	//bitMapFontTexture_9 = TextureManager::Load("cunt/9.png");

	// ビュー・プロジェクションの初期化
	worldTransform_.Initialize();
	viewProjection_.Initialize();


	//// ビットマップフォントの位置設定（右上の座標）
	//Vector3 fontPosition = {1280 - 100, 720 - 50, 0}; // スクリーンサイズが1280x720の場合
	//bitMapFont_->SetPosition(fontPosition);


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

	enemy_ = new Enemy();
	enemy_->Initialize(modelEnemy_, &viewProjection_, enmyPosition);

	// ブロックの生成
	GenerateBlocks();

	// カメラコントローラの生成と初期化
	cameraController_ = new CameraController();
	cameraController_->Initialize(bitMapFontTexture_);
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
	enemy_->Update();

}

void GameScene::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);
	// 背景スプライトの描画処理...
	Sprite::PostDraw();

	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();

	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	// プレイヤーの描画
	player_->Draw();

	// 敵の描画
	enemy_->Draw();

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
	// 前景スプライトの描画処理...
	Sprite::PostDraw();
}
