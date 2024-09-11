#pragma once

#include "MathUtilityForText.h"
#include "Vector3.h"
#include "ViewProjection.h"
#include "Player.h"
#include <algorithm>
#include <iostream>
#include <Model.h>

class Player;

class CameraController {
public:


	struct Rect {
		float left = 0.0f;
		float right = 1.0f;
		float bottom = 0.0f;
		float top = 1.0f;
	};

	// 座標補間割合
	static inline const float kInterpolationRate = 0.1f; // 補間率を設定
	static inline const float kVelocityBias = 30.0f;     // 速度バイアス

	Rect movableArea_ = {0, 100, 0, 100};
	Rect margin_ = {0, 0, 0, 0}; // 各方向へのマージンを追加
	Player* target_ = nullptr;

	void Initialize(Model* model);
	void Update();
	void Reset();

	void SetMovableArea(const Rect& area) { movableArea_ = area; }
	void SetMargin(const Rect& margin) { margin_ = margin; } // マージン設定メソッドを追加
	void setTarget(Player* target) { target_ = target; }

	const ViewProjection& GetViewProjection() const { return viewProjection_; }

	void Draw();                               // このままにします
	void SetPosition(const Vector3& position); // 位置設定メソッドを追加

private:
	
	ViewProjection viewProjection_;
	Vector3 targetOffset_ = {0, 0, -15.0f};
	Vector3 targetPosition_; // 目標座標を追加


	WorldTransform worldTransform_;

	Vector3 fontPosition_; // 位置を格納する変数

	uint32_t number2[5] = {0};
	uint32_t number1 = 0;

	uint32_t fontposX[5] = {0}; // 配列のサイズを変更
	uint32_t fontposY = 20;

	uint32_t scoretotal = 0;

	Model* model_ = nullptr;
};