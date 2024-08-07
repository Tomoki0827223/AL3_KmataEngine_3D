#include "TitleScene.h"
#include <numbers>

TitleSence::TitleSence() {}

TitleSence::~TitleSence() { delete titlemodel_; }

void TitleSence::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	titlemodel_ = Model::CreateFromOBJ("SugoiMan", true);
	titleWorldTransform_.Initialize();
	viewProjection_.Initialize();
	Timer_ = 0.0f;

	// タイトルを近づける
	titleWorldTransform_.translation_ = {-1.0f, 1.0f, -40.0f}; // z値を調整して近づける
}

void TitleSence::Update() {

	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		finished_ = true;
	}

	Timer_ += 1.0f / 90.0f;
	float param = std::cosf(2.0f * std::numbers::pi_v<float> * Timer_ / kWalklMotionTime);
	float oscillation = kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	titleWorldTransform_.translation_.y = kWalkMotionAngleStart + oscillation;

	// 行列計算
	titleWorldTransform_.UpdateMatrix();
}

void TitleSence::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Model::PreDraw(commandList);

	titlemodel_->Draw(titleWorldTransform_, viewProjection_);

	Model::PostDraw();
}
