#define NOMINMAX
#include "Player.h"
#include "Input.h"
#include "MapChipField.h"
#include "MathUtilityForText.h"
#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <numbers>
#include <DebugText.h>

void Player::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {


	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
	model_ = model;
	viewProjection_ = viewProjection;

}

void Player::Update() {

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
	MovePlayer();
	CollisionMapInfo collisionMapInfo;
	collisionMapInfo.movement = velocity_;
	CheckMapCollision(collisionMapInfo);
	
	JumpTranformMove(collisionMapInfo);
	CeilingContact(collisionMapInfo);
	worldTransform_.UpdateMatrix();
}

void Player::Draw() {

	// 3Dモデルを描画
	model_->Draw(worldTransform_, *viewProjection_);
}

void Player::MovePlayer() {
	// 移動入力
	// 接地状態
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
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kLimitRunSpeed;
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
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kLimitRunSpeed;
				}
			}
			// 加速/減速
			velocity_.x += acceleration.x;
			velocity_.y += acceleration.y;
			velocity_.z += acceleration.z;

			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			velocity_.x *= (1.0f - kAttenuation);
			velocity_.y *= (1.0f - kAttenuation);
			velocity_.z *= (1.0f - kAttenuation);
		}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ初速
			velocity_.x += 0;
			velocity_.y += kJumpAcceleration;
			velocity_.z += 0;
			// 空中
		}
	} else {
		// 落下速度
		velocity_.x += 0;
		velocity_.y += -kGravityAcceleration;
		velocity_.z += 0;
		// 落下速度制限
		velocity_.y = (std::max)(velocity_.y, -kLimitFallSpeed);
	}

	//// 旋回の更新
	//turnController_.UpdateTurn(3.0f / 60.0f); // 60FPSのフレームタイム
	//// 自キャラの角度を設定する
	//worldTransform_.rotation_.y = turnController_.GetCurrentRotationY();

	TurnControll();

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
			worldTransform_.translation_.y = 1.0f;
			// 摩擦で横方向速度が減衰する
			velocity_.x *= (1.0f - kAttenuation);
			// 下方向速度でリセット
			velocity_.y = 0.0f;
			// 接地状態に移行
			onGround_ = true;
		}
	}
}


//void Player::HandleCeilingCollision(const CollisionMapInfo& info) {
//	if (info.hitCeilingFlag) {
//		// 天井に触れた時の挙動を定義する
//		if (info.movement.y > 0.0f) {
//			// 天井の上にいる場合、下方向に落下する
//			worldTransform_.translation_.y -= info.movement.y;
//			velocity_.y = 0.0f; // 落下中は速度を0にすることで停止
//		}
//	}
//}
//
//
//
//void Player::HandleWallCollision(const CollisionMapInfo& info) {
//	if (info.wallContactFlag) {
//		// 壁に触れた時の挙動を定義する
//		if (info.movement.x != 0.0f) {
//			// 壁と左右方向に接触した場合、反射する
//			worldTransform_.translation_.x -= info.movement.x;
//			velocity_.x = -velocity_.x * 0.5f; // 反射後の速度を調整する
//
//			// 壁に触れた際の停止の処理
//			velocity_.x = 0.0f; // 速度を0にすることで停止
//		}
//	}
//}
//
//


void Player::ApplyCollisionResultAndMove(const CollisionMapInfo& info) {
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

	// 上昇アリ？
	if (info.movement.y <= 0) {
		return;
	}
	// 移動後４つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Vector3(0, info.movement.y, 0), static_cast<Corner>(i));
	}
	MapChipType mapChipType;
	
	// 真上の当たり判定を行う
	bool hit = false;
	
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlank) {
		hit = true;
	}
	
	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	
	if (mapChipType == MapChipType::kBlank) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, info.movement.y + 0.1f, 0) + Vector3(0, kHeight / 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.movement.y = (std::max)(0.0f, (rect.bottom - worldTransform_.translation_.y) - ((kHeight / 2.0f) + kBlank));
		// 天井に当たったことを記録する
		info.hitCeilingFlag = true;
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

	//bool hit = false;

	//// 下方向の各角の位置を基に衝突判定を行う
	//MapChipField::IndexSet indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);

	//MapChipType mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	//if (mapChipType == MapChipType::kBlock) {
	//	hit = true;
	//	MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
	//	info.movement.y = (std::min)(0.0f, rect.bottom - worldTransform_.translation_.y);
	//	info.landingFlag = true;
	//}

	//indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);

	//mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);

	//if (mapChipType == MapChipType::kBlock) {
	//	hit = true;
	//	MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
	//	info.movement.y = (std::min)(0.0f, rect.bottom - worldTransform_.translation_.y);
	//	info.landingFlag = true;
	//}


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
