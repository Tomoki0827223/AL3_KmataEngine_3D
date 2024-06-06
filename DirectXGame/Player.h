#pragma once

#include <Model.h>
#include <WorldTransform.h>

class Player {

public:

	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	void Update();

	void Draw();

private:

	WorldTransform worldTransform_;

	ViewProjection* viewProjection_ = nullptr;

	Model* model_ = nullptr;

	uint32_t textureHandle_ = 0u;
};

class PLayer {};
