#pragma once
#include "Vector4.h"
#include <d3d12.h>
#include <wrl.h>

// 定数バッファ用データ構造体
struct ConstBufferDataObjectColor {
	Vector4 color_;
};

// オブジェクト個別のカラー指定
class ObjectColor {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 行列を転送する
	/// </summary>
	void TransferMatrix();

	/// <summary>
	/// グラフィックスコマンドを積む
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="rootParameterIndex">ルートパラメータインデックス</param>
	void SetGraphicsCommand(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex) const;

	/// <summary>
	/// アクセッサ
	/// </summary>
	void SetColor(const Vector4& color) { color_ = color; }

private:
	Vector4 color_ = {1, 1, 1, 1};
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;
	// マッピング済みアドレス
	ConstBufferDataObjectColor* constMap_ = nullptr;

	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// マッピングする
	/// </summary>
	void Map();
};
