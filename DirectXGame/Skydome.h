#include "Model.h"
#include "Viewprojection.h"
#include "WorldTransform.h"

class Skydome {
public:
	void Initialize(Model* model, ViewProjection* viewProjection);

	void Update();

	void Draw();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	ViewProjection* viewProjection_ = nullptr;
	// モデル
	Model* model_ = nullptr;
};