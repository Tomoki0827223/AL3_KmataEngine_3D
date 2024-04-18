#pragma once
#include "Input.h"
#include "ViewProjection.h"

/// <summary>
/// デバッグ用カメラ
/// </summary>
class DebugCamera {
	// カメラ注視点までの距離
	static const float distance_;

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	DebugCamera(int window_width, int window_height);

	// 更新
	void Update();

	/// <summary>
	/// ビュープロジェクションを取得
	/// </summary>
	/// <returns>ビュープロジェクション</returns>
	const ViewProjection& GetViewProjection() { return viewProjection_; }

	/// <summary>
	/// プロジェクション行列計算用のメンバ設定関数群
	/// </summary>
	void SetFovAngleY(float value) { viewProjection_.fovAngleY = value; }
	void SetAspectRatio(float value) { viewProjection_.aspectRatio = value; }
	void SetNearZ(float value) { viewProjection_.nearZ = value; }
	void SetFarZ(float value) { viewProjection_.farZ = value; }

private:
	// 入力クラスのポインタ
	Input* input_;
	// スケーリング
	float scaleX_ = 1.0f;
	float scaleY_ = 1.0f;
	// ビュープロジェクション
	ViewProjection viewProjection_;
	// 回転行列
	Matrix4x4 matRot_;

	/// <summary>
	/// 行列更新
	/// </summary>
	void UpdateMatrix();
};
