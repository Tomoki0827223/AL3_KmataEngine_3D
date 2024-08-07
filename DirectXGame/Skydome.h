#pragma
#include "Model.h"
#include "WorldTransform.h"

class Skydome {

public:
	void Initialize(Model* model, ViewProjection* viewProjection);

	void Update();

	void Draw();

private:
	WorldTransform Sky_worldTransform_;

	Model* Sky_Model_ = nullptr;

	// テクスチャーハンドル
	uint32_t Sky_textureHandle_ = 0u;

	// ビューポート
	ViewProjection* Sky_viewProjection_ = nullptr;
};
