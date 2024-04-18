#pragma once

#include <d3d12.h>
#include <memory>
#include <wrl.h>

/// <summary>
/// 地形共用部
/// </summary>
class TerrainCommon {
private: // エイリアス
	// Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: // サブクラス
	// パイプラインセット
	struct PipelineSet {
		// ルートシグネチャ
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		// パイプラインステートオブジェクト
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	};

	/// <summary>
	/// ルートパラメータ番号
	/// </summary>
	enum class RoomParameter {
		kWorldTransform, // ワールド変換行列
		kViewProjection, // ビュープロジェクション変換行列
		kTexture,        // テクスチャ
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	void CreateGraphicsPipeline();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

private:
	// パイプラインセット
	PipelineSet pipelineSet_;
};
