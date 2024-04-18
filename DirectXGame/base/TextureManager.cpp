#include "TextureManager.h"
#include "StringUtility.h"
#include <DirectXTex.h>
#include <cassert>
#include <format>

using namespace DirectX;

uint32_t TextureManager::Load(const std::string& fileName) {
	return TextureManager::GetInstance()->LoadInternal(fileName);
}

bool TextureManager::Unload(uint32_t textureHandle) {
	return TextureManager::GetInstance()->UnloadInternal(textureHandle);
}

TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

void TextureManager::Initialize(ID3D12Device* device, std::string directoryPath) {
	assert(device);

	device_ = device;
	directoryPath_ = directoryPath;

	// デスクリプタサイズを取得
	sDescriptorHandleIncrementSize_ =
	    device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 全テクスチャリセット
	ResetAll();
}

void TextureManager::ResetAll() {
	HRESULT result = S_FALSE;

	// デスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // シェーダから見えるように
	descHeapDesc.NumDescriptors = kNumDescriptors; // シェーダーリソースビュー1つ
	result = device_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descriptorHeap_)); // 生成
	assert(SUCCEEDED(result));

	// 全テクスチャを初期化
	for (size_t i = 0; i < kNumDescriptors; i++) {
		textures_[i].resource.Reset();
		textures_[i].cpuDescHandleSRV.ptr = 0;
		textures_[i].gpuDescHandleSRV.ptr = 0;
		textures_[i].name.clear();
	}
	useTable_.Reset();
}

const D3D12_RESOURCE_DESC TextureManager::GetResoureDesc(uint32_t textureHandle) {

	assert(textureHandle < textures_.size());
	Texture& texture = textures_.at(textureHandle);
	return texture.resource->GetDesc();
}

void TextureManager::SetGraphicsRootDescriptorTable(
    ID3D12GraphicsCommandList* commandList, UINT rootParamIndex,
    uint32_t textureHandle) { // デスクリプタヒープの配列
	assert(textureHandle < textures_.size());
	ID3D12DescriptorHeap* ppHeaps[] = {descriptorHeap_.Get()};
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// シェーダリソースビューをセット
	commandList->SetGraphicsRootDescriptorTable(
	    rootParamIndex, textures_[textureHandle].gpuDescHandleSRV);
}

uint32_t TextureManager::LoadInternal(const std::string& fileName) {

	// 読み込み済みテクスチャを検索
	auto it = std::find_if(textures_.begin(), textures_.end(), [&](const auto& texture) {
		return texture.name == fileName;
	});
	if (it != textures_.end()) {
		// 読み込み済みテクスチャの要素番号を取得
		return static_cast<uint32_t>(std::distance(textures_.begin(), it));
	}

	// 書き込むテクスチャの参照
	uint32_t handle = uint32_t(useTable_.FindFirst());
	assert(handle < kNumDescriptors);

	Texture& texture = textures_.at(handle);
	texture.name = fileName;

	// ディレクトリパスとファイル名を連結してフルパスを得る
	bool currentRelative = false;
	if (2 < fileName.size()) {
		currentRelative = (fileName[0] == '.') && (fileName[1] == '/');
	}
	std::string fullPath = currentRelative ? fileName : directoryPath_ + fileName;

	// ユニコード文字列に変換
	wchar_t wfilePath[256];
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath, _countof(wfilePath));

	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// WICテクスチャのロード
	result = LoadFromWICFile(wfilePath, WIC_FLAGS_NONE, &metadata, scratchImg);
	if (FAILED(result)) {
		auto message = std::format(
		    L"テクスチャ「{0}」"
		    "の読み込みに失敗しました。\n指定したパスが正しいか、必須リソースのコピー"
		    "を忘れていないか確認してください。",
		    ConvertStringMultiByteToWide(fileName));
		MessageBoxW(nullptr, message.c_str(), L"Not found texture", 0);
		assert(false);
		exit(1);
	}

	ScratchImage mipChain{};
	// ミップマップ生成
	result = GenerateMipMaps(
	    scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
	    TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	// 読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);

	// リソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
	    metadata.format, metadata.width, (UINT)metadata.height, (UINT16)metadata.arraySize,
	    (UINT16)metadata.mipLevels);

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps =
	    CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

	// テクスチャ用バッファの生成
	result = device_->CreateCommittedResource(
	    &heapProps, D3D12_HEAP_FLAG_NONE, &texresDesc,
	    D3D12_RESOURCE_STATE_GENERIC_READ, // テクスチャ用指定
	    nullptr, IID_PPV_ARGS(&texture.resource));
	assert(SUCCEEDED(result));

	// テクスチャバッファにデータ転送
	for (size_t i = 0; i < metadata.mipLevels; i++) {
		const Image* img = scratchImg.GetImage(i, 0, 0); // 生データ抽出
		result = texture.resource->WriteToSubresource(
		    (UINT)i,
		    nullptr,              // 全領域へコピー
		    img->pixels,          // 元データアドレス
		    (UINT)img->rowPitch,  // 1ラインサイズ
		    (UINT)img->slicePitch // 1枚サイズ
		);
		assert(SUCCEEDED(result));
	}

	// シェーダリソースビュー作成
	texture.cpuDescHandleSRV = CD3DX12_CPU_DESCRIPTOR_HANDLE(
	    descriptorHeap_->GetCPUDescriptorHandleForHeapStart(), handle,
	    sDescriptorHandleIncrementSize_);
	texture.gpuDescHandleSRV = CD3DX12_GPU_DESCRIPTOR_HANDLE(
	    descriptorHeap_->GetGPUDescriptorHandleForHeapStart(), handle,
	    sDescriptorHandleIncrementSize_);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{}; // 設定構造体
	D3D12_RESOURCE_DESC resDesc = texture.resource->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = (UINT)metadata.mipLevels;

	device_->CreateShaderResourceView(
	    texture.resource.Get(), // ビューと関連付けるバッファ
	    &srvDesc,               // テクスチャ設定情報
	    texture.cpuDescHandleSRV);

	useTable_.Set(handle);

	return handle;
}

bool TextureManager::UnloadInternal(uint32_t textureHandle) {
	// 範囲外
	if (textures_.size() <= textureHandle) {
		return false;
	}

	auto& texture = textures_[textureHandle];
	// 範囲内だけど読んでない場所
	assert(!texture.name.empty());

	// テクスチャ設定を解除
	texture.resource.Reset();
	texture.cpuDescHandleSRV.ptr = 0;
	texture.gpuDescHandleSRV.ptr = 0;
	texture.name.clear();
	useTable_.Reset(textureHandle);
	return true;
}

template<size_t kNumberOfBits> TextureManager::Bitset<kNumberOfBits>::Bitset() { Reset(); }

template<size_t kNumberOfBits> size_t TextureManager::Bitset<kNumberOfBits>::FindFirst() const {
	for (size_t wordIndex = 0; wordIndex < kCountOfWord; ++wordIndex) {
		int firstZero = std::countr_one(words_[wordIndex]);
		if (firstZero != kBitsPerWord) {
			return wordIndex * kBitsPerWord + firstZero;
		}
	}

	return kCountOfWord * kBitsPerWord;
}

template<size_t kNumberOfBits>
void TextureManager::Bitset<kNumberOfBits>::Set(size_t bitIndex, bool value) {
	assert(bitIndex < kNumberOfBits);
	uint64_t& word = GetWord(bitIndex);
	if (value) {
		word |= (uint64_t(1) << (bitIndex & kBitsPerWordMask));
	} else {
		word &= ~(uint64_t(1) << (bitIndex & kBitsPerWordMask));
	}
}

template<size_t kNumberOfBits> void TextureManager::Bitset<kNumberOfBits>::Reset() {
	std::memset(words_, 0, sizeof(words_));
}

template<size_t kNumberOfBits> void TextureManager::Bitset<kNumberOfBits>::Reset(size_t bitIndex) {
	Set(bitIndex, false);
}

template<size_t kNumberOfBits>
bool TextureManager::Bitset<kNumberOfBits>::Test(size_t bitIndex) const {
	assert(bitIndex < kNumberOfBits);
	return (GetWord(bitIndex) & (uint64_t(1) << (bitIndex & kBitsPerWordMask))) != 0;
}

template<size_t kNumberOfBits>
uint64_t& TextureManager::Bitset<kNumberOfBits>::GetWord(size_t bitIndex) {
	return words_[bitIndex >> kBitIndexToWordIndex];
}