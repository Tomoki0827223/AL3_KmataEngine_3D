#include "Player.h"
#include "Input.h"
#include "MapChipField.h"
#include "MathUtilityForText.h"
#include <algorithm>
#include <cassert>
#include <numbers>
#include <array>

Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {

	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0}, // kRightBottom
	    {-kWidth / 2.0f, -kHeight / 2.0f, 0}, // kLeftBottom
	    {+kWidth / 2.0f, +kHeight / 2.0f, 0}, // kRightTop
	    {-kWidth / 2.0f, +kHeight / 2.0f, 0}  // kLeft Top
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}


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

	if (onGround_) {
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotesionY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				acceleration.x += kAcceleration;
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotesionY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}
				acceleration.x -= kAcceleration;
			}
			velocity_.x += acceleration.x;
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}

		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_.y = kJumpAcceleration;
			onGround_ = false;
		}
	} else {
		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
	}

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

	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, -std::numbers::pi_v<float> / 2.0f};
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		worldTransform_.rotation_.y = easeInOutSine(turnFirstRotesionY_, destinationRotationY, (kTimeTurn - turnTimer_) / kTimeTurn);
	}

	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.UpdateMatrix();
}

void Player::Draw() { model_->Draw(worldTransform_, *viewProjection_, textureHandle_); }

void Player::HandleCeilingCollision([[maybe_unused]] const CollisionMapInfo& info) { velocity_.y = -1.0f; }

void Player::HandleWallCollision([[maybe_unused]] const CollisionMapInfo& info) { velocity_.x = velocity_.x * 0.5f; }

void Player::ApplyCollisionResultAndMove(const CollisionMapInfo& info) {
	if (info.hitCeilingFlag) {
		HandleCeilingCollision(info);
	}
	if (info.wallContactFlag) {
		HandleWallCollision(info);
	}
	if (info.landingFlag) {
		onGround_ = true;
		worldTransform_.translation_.y = 2.0f; // 着地位置の調整
		velocity_.y = 0.0f;
	} else {
		onGround_ = false;
	}

	worldTransform_.translation_ += info.movement;
}

void Player::CheckMapCollision(CollisionMapInfo& info) {

	// 上方向の衝突判定
	CheckMapCollisionUp(info);
	// 下方向の衝突判定
	CheckMapCollisionDown(info);
	// 右方向の衝突判定
	CheckMapCollisionRight(info);
	// 左方向の衝突判定
	CheckMapCollisionLeft(info);

	// 例: マップチップフィールドでの衝突判定
	// この部分に実際の衝突判定ロジックを実装する必要があります
	// ここでは簡単な例として床がy=0の高さにあると仮定します

	// 床との衝突判定（着地判定）
	if (worldTransform_.translation_.y + info.movement.y <= 0.0f) {
		info.landingFlag = true;
		info.movement.y = 0.0f; // 着地したのでy方向の移動を0にする
	}

	// 天井との衝突判定（上方向の判定）
	if (worldTransform_.translation_.y + info.movement.y >= 10.0f) { // 仮に天井がy=10の高さにあるとする
		info.hitCeilingFlag = true;
		info.movement.y = 0.0f; // 天井にぶつかったのでy方向の移動を0にする
	}

	// 壁との衝突判定（左右方向の判定）
	if (worldTransform_.translation_.x + info.movement.x <= -5.0f || worldTransform_.translation_.x + info.movement.x >= 5.0f) {
		info.wallContactFlag = true;
		info.movement.x = 0.0f; // 壁にぶつかったのでx方向の移動を0にする
	}
}

void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	// 上昇あり?
	if (info.movement.y <= 0) {
		return;
	}

	// 移動後の4つの角の座標の計算
	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement, static_cast<Corner>(i));
	}

	// 真上の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		// めり込みを排除する方向に移動量を設定する
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.y = (std::max)(0.0f, rect.bottom - positionsNew[kLeftTop].y);
		// 天井に当たったことを記録する
		info.hitCeilingFlag = true;
	}

	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, kHeight / 2.0f, 0));
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		// めり込みを排除する方向に移動量を設定する
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.y = (std::max)(0.0f, rect.bottom - positionsNew[kRightTop].y);
		// 天井に当たったことを記録する
		info.hitCeilingFlag = true;
	}

	if (hit) {
		info.movement.y = 0.0f; // 天井にぶつかったのでy方向の移動を0にする
	}
}

void Player::CheckMapCollisionDown(CollisionMapInfo& info) {
	// 降下あり?
	if (info.movement.y >= 0) {
		return;
	}

	// 移動後の4つの角の座標の計算
	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement, static_cast<Corner>(i));
	}

	// 真下の当たり判定を行う
	bool hit = false;

	// 左下点の判定
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		// めり込みを排除する方向に移動量を設定する
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.y = (std::min)(0.0f, rect.top - positionsNew[kLeftBottom].y);
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		// めり込みを排除する方向に移動量を設定する
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.y = (std::min)(0.0f, rect.top - positionsNew[kRightBottom].y);
	}

	if (hit) {
		info.landingFlag = true;
		info.movement.y = 0.0f; // 床にぶつかったのでy方向の移動を0にする
	}
}

void Player::CheckMapCollisionRight(CollisionMapInfo& info) {
	// 右方向への移動あり?
	if (info.movement.x <= 0) {
		return;
	}

	// 移動後の4つの角の座標の計算
	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement, static_cast<Corner>(i));
	}

	// 右方向の当たり判定を行う
	bool hit = false;

	// 右上点の判定
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		// めり込みを排除する方向に移動量を設定する
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.x = (std::max)(0.0f, rect.left - positionsNew[kRightTop].x);
	}

	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		// めり込みを排除する方向に移動量を設定する
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.x = (std::max)(0.0f, rect.left - positionsNew[kRightBottom].x);
	}

	if (hit) {
		info.wallContactFlag = true;
		info.movement.x = 0.0f; // 右の壁にぶつかったのでx方向の移動を0にする
	}
}

void Player::CheckMapCollisionLeft(CollisionMapInfo& info) {
	// 左方向への移動あり?
	if (info.movement.x >= 0) {
		return;
	}

	// 移動後の4つの角の座標の計算
	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.movement, static_cast<Corner>(i));
	}

	// 左方向の当たり判定を行う
	bool hit = false;

	// 左上点の判定
	MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		// めり込みを排除する方向に移動量を設定する
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.x = (std::min)(0.0f, rect.right - positionsNew[kLeftTop].x);
	}

	// 左下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		// めり込みを排除する方向に移動量を設定する
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.x = (std::min)(0.0f, rect.right - positionsNew[kLeftBottom].x);
	}

	if (hit) {
		info.wallContactFlag = true;
		info.movement.x = 0.0f; // 左の壁にぶつかったのでx方向の移動を0にする
	}
}
