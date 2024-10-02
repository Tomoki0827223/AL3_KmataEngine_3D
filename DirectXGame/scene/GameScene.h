#pragma once
#include "AABB.h"
#include "Audio.h"
#include "CameraController.h"
#include "DethParticles.h"
#include "DebugCamera.h"
#include "DethParticles.h"
#include "DirectXCommon.h"
#include "Enemy.h"
#include "Input.h"
#include "MapChipField.h"
#include "Model.h"
#include "Player.h"
#include "Skydome.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

/// <summary>
/// ゲームシーン
/// </summary>

class DethPertcles;

// ゲームのフェーズ(型)
enum class Phase {
	kPlay,  // ゲームプレイ
	kDeath, // デス演出
};

class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	Player* player_ = nullptr;
	Skydome* skydome = nullptr;
	MapChipField* mapChipField_ = nullptr;
	DeathParticles* dethParticles_ = nullptr;

	Model* modelEnemy_ = nullptr;
	Model* modelPlayer_ = nullptr;
	Model* modelBlock_ = nullptr;
	Model* modelSkydome_ = nullptr;
	Model* modelParticles_ = nullptr;

	Vector3 playerPos = {};
	]
	CameraController* cameraController_;

	void CheckAllCollisions();

	void ChangePhase();

	std::list<Enemy*> enemies_;

	// ワールドビュー
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	WorldTransform worldTransform_;
	ViewProjection viewProjection_;

	bool finished_ = false;

	bool IsFinished() const { return finished_; }

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
	/// <summary>
	/// 表示ブロックの生成
	/// </summary>
	void GenerateBlocks();

private: // メンバ変数
	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	DeathParticles* deathParticles_ = nullptr;

	Phase phase_;

	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
