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

	// 移動入力
	if (onGround_) {
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

			// 左右加速
			Vector3 acceleration = {};

			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x += kAcceleration;

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					// 旋回開始時の角度
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマー
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				// 右移動中の左入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}

				acceleration.x -= kAcceleration;

				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					// 旋回開始時の角度
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマー
					turnTimer_ = kTimeTurn;
				}
			}
			// 加速/減速
			velocity_.x += acceleration.x;
			velocity_.y += acceleration.y;
			velocity_.z += acceleration.z;

			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		} else {
			// 非入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}

		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ初速
			//			velocity_ += Vector3(0, kJumpAcceleration, 0);
			velocity_.x += 0;
			velocity_.y += kJumpAcceleration;
			velocity_.z += 0;
		}

	} else {
		// 落下速度
		//		velocity_ += Vector3(0, -kGravityAcceleration, 0);
		velocity_.x += 0;
		velocity_.y += -kGravityAcceleration;
		velocity_.z += 0;
		// 落下速度制限
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
	}

	// 左右の自キャラ角度テーブル
	float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
	// 状態に応じた角度を取得する
	float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
	// 自キャラの角度を設定する
	worldTransform_.rotation_.y = destinationRotationY;

	// 着地フラグ
	bool landing = false;

	// 地面との当たり判定
	// 下降中？
	if (velocity_.y < 0) {
		// Y座標が地面以下になったら着地
		if (worldTransform_.translation_.y <= 2.0f) {
			landing = true;
		}
	}

	// 接地判定
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		}
	} else {
		// 着地
		if (landing) {
			// めり込み排斥
			worldTransform_.translation_.y = 2.0f;
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenuation);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 接地状態に移行
			onGround_ = true;
		}
	}

	// 移動
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	// 行列計算
	worldTransform_.UpdateMatrix();
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}



void Player::Draw() { model_->Draw(worldTransform_, *viewProjection_, textureHandle_); }


void Player::HandleCeilingCollision(const CollisionMapInfo& info) {
	if (info.hitCeilingFlag) {
		// 天井に触れた時の挙動を定義する
		if (info.movement.y > 0.0f) {
			// 天井の上にいる場合、下方向に反射する
			worldTransform_.translation_.y -= info.movement.y;
			velocity_.y = -velocity_.y * 0.5f; // 反射後の速度を調整する

			// 天井に触れた際の停止の処理
			velocity_.y = 0.0f; // 速度を0にすることで停止
		}
	}
}

void Player::HandleWallCollision(const CollisionMapInfo& info) {
	if (info.wallContactFlag) {
		// 壁に触れた時の挙動を定義する
		if (info.movement.x != 0.0f) {
			// 壁と左右方向に接触した場合、反射する
			worldTransform_.translation_.x -= info.movement.x;
			velocity_.x = -velocity_.x * 0.5f; // 反射後の速度を調整する

			// 壁に触れた際の停止の処理
			velocity_.x = 0.0f; // 速度を0にすることで停止
		}
	}
}


void Player::ApplyCollisionResultAndMove(const CollisionMapInfo& info) {
	// 衝突情報を適用してプレイヤーの移動を行う
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
		info.movement.y = (std::max)(0.0f, rect.bottom - worldTransform_.translation_.y - kHeight / 2.0f);
		info.hitCeilingFlag = true;
	}

	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.y = (std::max)(0.0f, rect.bottom - worldTransform_.translation_.y - kHeight / 2.0f);
		info.hitCeilingFlag = true;
	}

	if (hit) {
		velocity_.y = 0.0f; // 天井に衝突したらy方向の速度を0にする
		worldTransform_.translation_.y += info.movement.y;
		info.movement.y = 0.0f;
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
	} else {
		onGround_ = false; // 地面に接触していない場合
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
		velocity_.x = 0.0f; // 衝突したらx方向の速度を0にする
		worldTransform_.translation_.x += info.movement.x;
		info.movement.x = 0.0f;
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
