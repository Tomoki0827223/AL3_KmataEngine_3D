#pragma once
#include "MathUtilityForText.h"
#include "Model.h"
#include "affine.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "affine.h"
#include <algorithm>
#include <array>

class DeathParticles {

public:
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	void Update();

	void Draw();

	bool IsFinished() const { return isFinished_; }

private:
	// モデルのポインタ
	Model* model_ = nullptr;

	// ビュープロジェクションのポインタ
	ViewProjection* viewProjection_ = nullptr;

	// パーティクルの個数
	static inline const uint32_t kNumParticles = 8;

	std::array<WorldTransform, kNumParticles> worldTransform_;

	static inline const float kDuration = 2.0f;
	static inline const float kSpeed = 0.05f;
	static inline const float kAngleUint = 2.0f * 3.14f / kNumParticles;
	// 終了フラグ
	bool isFinished_ = false;
	float counter_ = 0.0f;
	// 色変更オブジェクト
	ObjectColor objectColor_;
	Vector4 color_;
};