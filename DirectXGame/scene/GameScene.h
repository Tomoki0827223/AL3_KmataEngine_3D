#pragma once

#include "AABB.h"
#include "Audio.h"
#include "AxisIndicator.h"
#include "CameraController.h"
#include "DebugCamera.h"
#include "DethParticles.h"
#include "DirectXCommon.h"
#include "Enemy.h"
#include "Input.h"
#include "MapChipField.h"
#include "Model.h"
#include "Player.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "affine.h"
#include <cassert>

/// <summary>
/// ゲームシーン
/// </summary>

class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	Model* model_ = nullptr;
	Model* playerResorces_ = nullptr;
	Model* modelEnemy_ = nullptr;
	Model* modelParticles_ = nullptr;

	WorldTransform worldTransform_;

	ViewProjection viewProjection_;

	MapChipField* mapChipField_;

	CameraController* cameraController_;

	Player* player_ = nullptr;

	std::list<Enemy*> enemies_;

	void GenerateBlocks();

	void CheckAllCollisions();

	// 縦横ブロック配列
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	int isDebugCameraActive_ = 0u;
	DebugCamera* debugCamera_ = nullptr;

	DeathParticles* dethParticles_ = nullptr;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
