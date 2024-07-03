#include "Player.h"
#include "Input.h"
#include "MapChipField.h"
#include "MathUtilityForText.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <numbers>

void Player::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	model_ = model;
	viewProjection_ = viewProjection;
}

void Player::Update() {
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.movement = velocity_;
	CheckMapCollision(collisionMapInfo);
	ApplyCollisionResultAndMove(collisionMapInfo);

	// 接地状態の処理と重力の影響を考慮
	if (onGround_) {
		Vector3 acceleration = {};
		if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
			// 右に加速
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
			// 左に加速
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
		// 空中にいる場合、重力が働く
		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
	}

	// 着地判定と位置の更新
	bool landing = false;
	if (velocity_.y < 0) {
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}

	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}
	} else {
		if (landing) {
			worldTransform_.translation_.y = 2.0f;
			velocity_.x *= (1.0f - kAttenuation);
			velocity_.y = 0.0f;
			onGround_ = true;
		}
	}

	// プレイヤーの向き変更の処理
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, -std::numbers::pi_v<float> / 2.0f};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		worldTransform_.rotation_.y = easeInOutSine(turnFirstRotationY_, destinationRotationY, (kTimeTurn - turnTimer_) / kTimeTurn);
	}

	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.UpdateMatrix();
}

void Player::Draw() {}


void Player::HandleCeilingCollision(const CollisionMapInfo& info) {
	// 天井との衝突処理を行う
	if (info.hitCeilingFlag) {
		// 何かしらの処理を行う
		// 例えば、天井に触れた時の挙動を定義する
		// info.movement などの情報を使用して適切な処理を行う
	}
}

void Player::HandleWallCollision(const CollisionMapInfo& info) {
	// 壁との衝突処理を行う
	if (info.wallContactFlag) {
		// 何かしらの処理を行う
		// 例えば、壁に触れた時の挙動を定義する
		// info.movement などの情報を使用して適切な処理を行う
	}
}

void Player::ApplyCollisionResultAndMove(const CollisionMapInfo& info) {
	// 衝突情報を適用してプレイヤーの移動を行う
	// ここで、info.movement に基づいて実際に移動を行うロジックを記述する
	worldTransform_.translation_ += info.movement;
}


void Player::CheckMapCollision(CollisionMapInfo& info) {
	CheckMapCollisionUp(info);
	CheckMapCollisionDown(info);
	CheckMapCollisionRight(info);
	CheckMapCollisionLeft(info);

	// 地面と天井の判定
	if (worldTransform_.translation_.y + info.movement.y <= 0.0f) {
		info.landingFlag = true;
		info.movement.y = 0.0f;
	}

	if (worldTransform_.translation_.y + info.movement.y >= 10.0f) {
		info.hitCeilingFlag = true;
		info.movement.y = 0.0f;
	}

	// 壁との衝突判定
	if (worldTransform_.translation_.x + info.movement.x <= -5.0f || worldTransform_.translation_.x + info.movement.x >= 5.0f) {
		info.wallContactFlag = true;
		info.movement.x = 0.0f;
	}
}

void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	// 上方向に移動していない場合は処理しない
	if (info.movement.y <= 0.0f) {
		return;
	}

	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement, static_cast<Corner>(i));
	}

	bool hit = false;

	// 上方向の各角の位置を基に衝突判定を行う
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.y = (std::max)(0.0f, rect.top - worldTransform_.translation_.y);
		info.hitCeilingFlag = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.y = (std::max)(0.0f, rect.top - worldTransform_.translation_.y);
		info.hitCeilingFlag = true;
	}

	if (hit) {
		velocity_.y = 0.0f; // 衝突したらy方向の速度を0にする
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo& info) {
	// 下方向に移動していない場合は処理しない
	if (info.movement.y >= 0.0f) {
		return;
	}

	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement, static_cast<Corner>(i));
	}

	bool hit = false;

	// 下方向の各角の位置を基に衝突判定を行う
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.y = (std::min)(0.0f, rect.bottom - worldTransform_.translation_.y);
		info.landingFlag = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.y = (std::min)(0.0f, rect.bottom - worldTransform_.translation_.y);
		info.landingFlag = true;
	}

	if (hit) {
		onGround_ = true;   // 着地したらonGround_フラグを立てる
		velocity_.y = 0.0f; // y方向の速度を0にする
	}
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
		info.movement.x = (std::min)(0.0f, rect.right - worldTransform_.translation_.x);
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.x = (std::min)(0.0f, rect.right - worldTransform_.translation_.x);
	}

	if (hit) {
		velocity_.x = 0.0f; // 衝突したらx方向の速度を0にする
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
		velocity_.x = 0.0f; // 衝突したらx方向の速度を0にする
	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 position;
	switch (corner) {
	case kRightBottom:
		position = center + Vector3(kWidth / 2.0f, -kHeight / 2.0f, 0.0f);
		break;
	case kLeftBottom:
		position = center + Vector3(-kWidth / 2.0f, -kHeight / 2.0f, 0.0f);
		break;
	case kRightTop:
		position = center + Vector3(kWidth / 2.0f, kHeight / 2.0f, 0.0f);
		break;
	case kLeftTop:
		position = center + Vector3(-kWidth / 2.0f, kHeight / 2.0f, 0.0f);
		break;
	default:
		position = center;
		break;
	}
	return position;
}
