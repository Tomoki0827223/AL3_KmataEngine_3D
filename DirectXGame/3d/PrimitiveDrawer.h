#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "ViewProjection.h"
#include <array>
#include <cstdint>
#include <d3d12.h>
#include <memory>
#include <string>
#include <wrl.h>

// 基本プリミティブ描画
class PrimitiveDrawer {
public:
	// 線分の最大数
	static const UINT kMaxLineCount = 4096;
	// 線分の頂点数
	static const UINT kVertexCountLine = 2;
	// 線分のインデックス数
	static const UINT kIndexCountLine = 0;

	// ブレンドモード
	enum class BlendMode {
		kBlendModeNone, //!< ブレンドなし
		kBlendModeNormal, //!< 通常αブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
		kBlendModeAdd,    //!< 加算。Src * SrcA + Dest * 1
		kBlendModeSubtract, //!< 減算。Dest * 1 - Src * SrcA
		kBlendModeMultily,  //!< 乗算。Src * 0 + Dest * Src
		kBlendModeScreen,   //!< スクリーン。Src * (1 - Dest) + Dest * 1

		// 利用してはいけない
		kCountOfBlendMode,
	};

	// 頂点データ構造体
	struct VertexPosColor {
		Vector3 pos;   // xyz座標
		Vector4 color; // RGBA
	};

	// パイプラインセット
	struct PipelineSet {
		// ルートシグネチャ
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		// パイプラインステートオブジェクト
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	};

	// メッシュ
	struct Mesh {
		// 頂点バッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff;
		// インデックスバッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff;
		// 頂点バッファビュー
		D3D12_VERTEX_BUFFER_VIEW vbView{};
		// インデックスバッファビュー
		D3D12_INDEX_BUFFER_VIEW ibView{};
		// 頂点バッファマップ
		VertexPosColor* vertMap = nullptr;
		// インデックスバッファマップ
		uint16_t* indexMap = nullptr;
	};

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static PrimitiveDrawer* GetInstance();

	/// <summary>
	/// リソース生成
	/// </summary>
	/// <param name="size">サイズ</param>
	/// <returns>生成したリソース</returns>
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateCommittedResource(UINT64 size);

	/// <summary>
	/// メッシュ生成
	/// </summary>
	/// <param name="vertexCount">頂点数</param>
	/// <param name="indexCount">インデックス数</param>
	/// <returns></returns>
	std::unique_ptr<Mesh> CreateMesh(UINT vertexCount, UINT indexCount);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 3D線分の描画
	/// </summary>
	/// <param name="p1">始点座標</param>
	/// <param name="p2">終点座標</param>
	/// <param name="color">色(RGBA)</param>
	void DrawLine3d(const Vector3& p1, const Vector3& p2, const Vector4& color);

	/// <summary>
	/// リセット
	/// </summary>
	void Reset();

	/// <summary>
	/// ビュープロジェクションのセット
	/// </summary>
	/// <param name="viewProjection"></param>
	void SetViewProjection(const ViewProjection* viewProjection) {
		viewProjection_ = viewProjection;
	}

private:
	PrimitiveDrawer() = default;
	~PrimitiveDrawer() = default;
	PrimitiveDrawer(const PrimitiveDrawer&) = delete;
	PrimitiveDrawer& operator=(const PrimitiveDrawer&) = delete;

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	std::unique_ptr<PipelineSet>
	    CreateGraphicsPipeline(D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType, BlendMode blendMode);

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	void CreateGraphicsPipelines();

	/// <summary>
	/// 各種メッシュ生成
	/// </summary>
	void CreateMeshes();

	// 線分
	std::unique_ptr<Mesh> line_;
	// 線分の使用インデックス
	uint32_t indexLine_ = 0;
	// 参照するビュープロジェクション
	const ViewProjection* viewProjection_ = nullptr;
	// ブレンドモード
	BlendMode blendMode_ = BlendMode::kBlendModeNormal;
	// パイプラインセット
	std::array<std::unique_ptr<PipelineSet>, (uint16_t)BlendMode::kCountOfBlendMode>
	    pipelineSetLines_;
};
