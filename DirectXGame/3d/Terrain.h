#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <string>
#include <vector>
#include <wrl.h>

/// <summary>
/// 地形
/// </summary>
class Terrain {
private: // エイリアス
	// Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: // サブクラス
	// 頂点データ構造体（テクスチャあり）
	struct VertexPosNormalUv {
		Vector3 pos;    // xyz座標
		Vector3 normal; // 法線ベクトル
		Vector2 uv;     // uv座標
	};

public:
	// デフォルト横方向頂点数
	static const uint32_t kDefaultVertexCountHorizontal;
	// デフォルト高さ
	static const float kDefaultHeight;
	// デフォルト幅
	static const float kDefaultModelWidth;

	static const size_t kSizePermutation = 256;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(
	    float modelWidth = kDefaultModelWidth, float modelDepth = kDefaultModelWidth,
	    float modelHeight = kDefaultHeight,
	    uint32_t vertexCountHorizontal = kDefaultVertexCountHorizontal,
	    uint32_t vertexCountVertical = kDefaultVertexCountHorizontal);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	/// <param name="viewProjection">ビュープロジェクション</param>
	/// <param name="textureHadle">テクスチャハンドル</param>
	void Draw(
	    const WorldTransform& worldTransform, const ViewProjection& viewProjection,
	    uint32_t textureHadle);

	/// <summary>
	/// 2Dパーリンノイズによる地形変動
	/// </summary>
	/// <param name="gridSize">1グリッドがまたぐ頂点数</param>
	void DeformRandom(uint32_t gridSize = 5);

	/// <summary>
	/// 頂点配列の取得
	/// </summary>
	/// <returns>頂点配列（前後×左右の二次元配列）</returns>
	const std::vector<std::vector<VertexPosNormalUv>>& GetVertices() const { return vertices_; }

private:
	// 横方向頂点数
	uint32_t vertexCountHorizontal_ = kDefaultVertexCountHorizontal;
	// 縦方向頂点数
	uint32_t vertexCountVertical_ = kDefaultVertexCountHorizontal;
	// モデル左右幅
	float modelWidth_ = kDefaultModelWidth;
	// モデル奥行幅
	float modelDepth_ = kDefaultModelWidth;
	// モデル高さ
	float modelHeight_ = kDefaultHeight;
	// 頂点配列
	std::vector<std::vector<VertexPosNormalUv>> vertices_;
	// 頂点インデックス配列
	std::vector<uint32_t> indices_;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;
	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};
	// 頂点バッファマップ
	VertexPosNormalUv* vertMap_ = nullptr;
	// インデックスバッファマップ
	uint32_t* indexMap_ = nullptr;
	// 勾配ベクトル
	std::vector<std::vector<Vector2>> gradients_;

	/// <summary>
	/// バッファ生成
	/// </summary>
	void CreateBuffers();

	/// <summary>
	/// バッファ転送
	/// </summary>
	void TransferBuffers();

	static float SmoothStep(float t) {
		// 改良パーリンノイズのフェード関数
		// 6t^5 - 15t^4 + 10t^3
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	/// <summary>
	/// パーリンノイズ
	/// </summary>
	/// <param name="x">X座標</param>
	/// <param name="y">Y座標</param>
	/// <returns>[0,1]の値</returns>
	float Perlin(float x, float y);
};
