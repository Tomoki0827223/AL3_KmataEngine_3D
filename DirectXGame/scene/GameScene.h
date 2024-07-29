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
#include "AABB.h"
#include "DethParticles.h"
#include "Skydome.h"

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

	Model* modelParticles_ = nullptr;

	// 天球
	Skydome* skydome_ = nullptr;
	// 3Dモデル
	Model* modelSkydome_ = nullptr;

	WorldTransform worldTransform_;

	ViewProjection viewProjection_;

	MapChipField* mapChipField_;

	CameraController* cameraController_;

	Player* player_ = nullptr;

	Model* modelEnemy_ = nullptr;

	std::list<Enemy*> enemies_;

	DeathParticles* deathParticles_ = nullptr;

	void GenerateBlocks();

	void CheckAllCollisions();

	void ChangePhase();

	// 縦横ブロック配列
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;

	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;

	// ゲームのフェーズ(型)
	enum class Phase {
		kPlay,  // ゲームプレイ
		kDeath, // デス演出
	};

	Phase phace_;

	bool finished_ = false;

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
