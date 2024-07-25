#include "Model.h"
#include "Vector3.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include <array>

#pragma once
class DethParticles {
public:
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);
	void Update();
	void Draw();

private:
	// パーティクルの個数
	static inline const uint32_t kNumParticles = 8;

	std::array<WorldTransform, kNumParticles> worldTransforms_;
	std::array<Vector3, kNumParticles> velocities_;
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// モデルのポインタ
	Model* model_ = nullptr;
	// ビュープロジェクションのポインタ
	ViewProjection* viewProjection_ = nullptr;
};