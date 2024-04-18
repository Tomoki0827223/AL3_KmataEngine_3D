#pragma once

#include "DirectXCommon.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <memory>
#include <string>

// 軸方向表示
class AxisIndicator {
public:
	// ビューポート矩形範囲
	static const float kViewPortTopLeftX;
	static const float kViewPortTopLeftY;
	static const float kViewPortWidth;
	static const float kViewPortHeight;
	static const float kCameraDistance;

	// モデル名
	static const std::string kModelName;

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static AxisIndicator* GetInstance();

	/// <summary>
	/// トレースするビュープロジェクションのセット
	/// </summary>
	/// <param name="targetViewProjection">トレースするビュープロジェクション</param>
	static void SetTargetViewProjection(const ViewProjection* targetViewProjection);

	/// <summary>
	/// 表示フラグのセット
	/// </summary>
	/// <param name="isVisible">表示フラグ</param>
	static void SetVisible(bool isVisible);

	/// /// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	AxisIndicator() = default;
	~AxisIndicator() = default;
	AxisIndicator(const AxisIndicator&) = delete;
	AxisIndicator& operator=(const AxisIndicator&) = delete;

	// DirectX基盤
	DirectXCommon* dxCommon_ = nullptr;
	// モデル
	std::unique_ptr<Model> model_;
	// ビュープロジェクション
	ViewProjection viewProjection_;
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// トレースするビュープロジェクション
	const ViewProjection* targetViewProjection_ = nullptr;
	// 表示フラグ
	bool isVisible_ = false;
};
