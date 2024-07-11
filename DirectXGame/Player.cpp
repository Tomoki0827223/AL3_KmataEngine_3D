#define NOMINMAX
#include "Player.h"
#include "Input.h"
#include "MapChipField.h"
#include "MathUtilityForText.h"
#include <DebugText.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <numbers>

void Player::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {

	assert(model);

	model_ = model;

	// textureHandle_ = textureHandle;

	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	viewProjection_ = viewProjection;
}

void Player::Update() {

	MovePlayer();

	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.movement = velocity_;

	CheckMapCollision(collisionMapInfo);

	CheckMapMove(collisionMapInfo);

	CeilingContact(collisionMapInfo);

	JumpTranformMove(collisionMapInfo);

	// 着地フラグ
	bool landing = false;

	// 地面とのあたり判定
	if (velocity_.y < 0) {

		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}

	// 接地判定
	if (onGround_) {

		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態の移行
			onGround_ = false;
		}

	} else {

		// 着地
		if (landing) {
			// めり込み排斥
			worldTransform_.translation_.y = 3.0f;
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenuation);
			// 下方向速度でリセット
			velocity_.y = 0.0f;
			// 接地状態に移行
			onGround_ = true;
		}
	}

	TurnControll();
	worldTransform_.UpdateMatrix();
}

void Player::Draw() {

	// 3Dモデルを描画
	model_->Draw(worldTransform_, *viewProjection_);
}

void Player::MovePlayer() {
	if (onGround_) {
		// 左右の移動処理
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {

				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}

				acceleration.x += kAcceleration;
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {

				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}

				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				acceleration.x -= kAcceleration;
			}
			velocity_.x += acceleration.x;
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}

		// ジャンプの処理
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_.y = kJumpAcceleration;
			onGround_ = false;
		}
	} else {
		// 空中での処理
		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

void Player::CheckMapMove(const CollisionMapInfo& info) {
	// 衝突情報を適用してプレイヤーの移動を行う
	worldTransform_.translation_ += info.movement;
}

void Player::CheckMapCollision(CollisionMapInfo& info) {

	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);
}

void Player::CheckMapCollisionUp(CollisionMapInfo& info) {

	// 上昇あり？
	if (info.movement.y <= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右上点の判定
	MapChipField::IndexSet indexSet2;
	indexSet2 = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet2.xIndex, indexSet2.yIndex);

	if (mapChipType == MapChipType::kBlock) {

		hit = true;
	}

	// ブロックにヒット
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, +kHeight / 2.0f, 0));

		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));

		// 衝突判定
		info.hitCeilingFlag = true;
	}

	// 衝突
	CeilingContact(info);
}

void Player::CheckMapCollisionDown(CollisionMapInfo& info) {

	// 下降あり？
	if (info.movement.y >= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement, static_cast<Corner>(i));
	}

	MapChipType mapChipType;

	// 真下の当たり判定を行う
	bool hit = false;

	// 左下点の判定
	MapChipField::IndexSet indexSet3;
	indexSet3 = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet3.xIndex, indexSet3.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	MapChipField::IndexSet indexSet4;
	indexSet4 = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet4.xIndex, indexSet4.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet3 = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, -kHeight / 3.0f, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet3.xIndex, indexSet3.yIndex);
		info.movement.y = std::min(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 3.0f + kBlank));
		info.landingFlag = true;
	} else {
		info.landingFlag = false;
	}

	CeilingContact(info);
}

void Player::CheckMapCollisionRight(CollisionMapInfo& info) {

	// 右方向に移動していない場合は処理しない
	if (info.movement.x <= 0.0f) {
		return;
	}

	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement, static_cast<Corner>(i));
	}

	bool hit = false;

	// 右方向の各角の位置を基に衝突判定を行う
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.x = (std::min)(0.0f, rect.left - worldTransform_.translation_.x - kWidth / 2.0f);
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.x = (std::min)(0.0f, rect.left - worldTransform_.translation_.x - kWidth / 2.0f);
	}

	if (hit) {
		velocity_.x = 0.0f; // 衝突したらx方向の速度を0にする
		worldTransform_.translation_.x += info.movement.x;
		info.movement.x = 0.0f;
	}
}

void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {

	// 左方向に移動していない場合は処理しない
	if (info.movement.x >= 0.0f) {
		return;
	}

	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement, static_cast<Corner>(i));
	}

	bool hit = false;

	// 左方向の各角の位置を基に衝突判定を行う
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.x = (std::max)(0.0f, rect.left - worldTransform_.translation_.x);
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.x = (std::max)(0.0f, rect.left - worldTransform_.translation_.x);
	}

	if (hit) {
		velocity_.x = 0.0f;                                // 衝突したらx方向の速度を0にする
		worldTransform_.translation_.x += info.movement.x; // 位置の更新を追加
		info.movement.x = 0.0f;
	}
}

void Player::CeilingContact(const CollisionMapInfo& info) {

	// 天井に当たった?
	if (info.hitCeilingFlag) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
	}
}

void Player::JumpTranformMove(const CollisionMapInfo& info) {

	// 移動
	worldTransform_.translation_.x += info.movement.x;
	worldTransform_.translation_.y += info.movement.y;
	worldTransform_.translation_.z += info.movement.z;
}

void Player::TurnControll() {

	// 旋回制御
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 3.0f / 60.0f;
		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		worldTransform_.rotation_.y = destinationRotationY;
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {

	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kleftTop
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];
}
