#pragma once

#include "Audio.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "MapChipField.h"
#include "Model.h"
#include "Player.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "CameraController.h"
#include "Enemy.h"

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

	WorldTransform worldTransform_;

	ViewProjection viewProjection_;

	MapChipField* mapChipField_;

	CameraController* cameraController_;

	Player* player_ = nullptr;

	Model* modelEnemy_ = nullptr;

	std::list<Enemy*> enemies_;

	void GenerateBlocks();

	void CheckAllCollisions();

	// 衝突判定関数
	bool IsCollision(const AABB& aabb1, const AABB& aabb2);

	// 縦横ブロック配列
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	int isDebugCameraActive_ = 0u;
	DebugCamera* debugCamera_ = nullptr;

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
