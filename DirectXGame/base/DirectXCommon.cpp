#include "DirectXCommon.h"
#include "DebugText.h"
#include <algorithm>
#include <cassert>
#include <thread>
#include <timeapi.h>
#include <vector>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Winmm.lib")

using namespace Microsoft::WRL;

namespace {
const uint32_t kNumRTVDescriptor = 4;
const uint32_t kLinearRTVStart = 2;
} // namespace

DirectXCommon* DirectXCommon::GetInstance() {
	static DirectXCommon instance;
	return &instance;
}

void DirectXCommon::Initialize(
    WinApp* winApp, int32_t backBufferWidth, int32_t backBufferHeight, bool enableDebugLayer) {
	// nullptrチェック
	assert(winApp);
	assert(4 <= backBufferWidth && backBufferWidth <= 4096);
	assert(4 <= backBufferHeight && backBufferHeight <= 4096);

	// sleepの分解能をあげておく
	timeBeginPeriod(1);

	winApp_ = winApp;
	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;
	reference_ = std::chrono::steady_clock::now();

	// DXGIデバイス初期化
	InitializeDXGIDevice(enableDebugLayer);

	// コマンド関連初期化
	InitializeCommand();

	// スワップチェーンの生成
	CreateSwapChain();

	// レンダーターゲット生成
	CreateFinalRenderTargets();

	// 深度バッファ生成
	CreateDepthBuffer();

	// フェンス生成
	CreateFence();
}

void DirectXCommon::PreDraw() {
	// バックバッファの番号を取得（2つなので0番か1番）
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	// リソースバリアを変更（表示状態→描画対象）
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
	    backBuffers_[bbIndex].Get(), D3D12_RESOURCE_STATE_PRESENT,
	    D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList_->ResourceBarrier(1, &barrier);

	// レンダーターゲット設定。SRGBフォーマットがデフォルト
	SetRenderTargets(true);

	// 全画面クリア
	ClearRenderTarget();
	// 深度バッファクリア
	ClearDepthBuffer();

	// ビューポートの設定
	CD3DX12_VIEWPORT viewport =
	    CD3DX12_VIEWPORT(0.0f, 0.0f, float(backBufferWidth_), float(backBufferHeight_));
	commandList_->RSSetViewports(1, &viewport);
	// シザリング矩形の設定
	CD3DX12_RECT rect = CD3DX12_RECT(0, 0, backBufferWidth_, backBufferHeight_);
	commandList_->RSSetScissorRects(1, &rect);
}

void DirectXCommon::PostDraw() {
	HRESULT result;

	// リソースバリアを変更（描画対象→表示状態）
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
	    backBuffers_[bbIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
	    D3D12_RESOURCE_STATE_PRESENT);
	commandList_->ResourceBarrier(1, &barrier);

	// 命令のクローズ
	commandList_->Close();

	// コマンドリストの実行
	ID3D12CommandList* cmdLists[] = {commandList_.Get()}; // コマンドリストの配列
	commandQueue_->ExecuteCommandLists(1, cmdLists);

	// バッファをフリップ。60fps固定のため、30fpsなどのモニタはティアリング覚悟で垂直同期無視
	static constexpr int32_t kThreasholdRefreshRate = 58;
	result = swapChain_->Present(refreshRate_ < kThreasholdRefreshRate ? 0 : 1, 0);
#ifdef _DEBUG
	if (FAILED(result)) {
		ComPtr<ID3D12DeviceRemovedExtendedData> dred;

		result = device_->QueryInterface(IID_PPV_ARGS(&dred));
		assert(SUCCEEDED(result));

		// 自動パンくず取得
		D3D12_DRED_AUTO_BREADCRUMBS_OUTPUT autoBreadcrumbsOutput{};
		result = dred->GetAutoBreadcrumbsOutput(&autoBreadcrumbsOutput);
		assert(SUCCEEDED(result));
	}
#endif

	// コマンドリストの実行完了を待つ
	commandQueue_->Signal(fence_.Get(), ++fenceVal_);
	if (fence_->GetCompletedValue() != fenceVal_) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence_->SetEventOnCompletion(fenceVal_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	// ウィンドウ閉じるとframeLatencyWaitableObject_をインクリメントする対象がいなくなって0のままになるからInfiniteにしない
	// 初期化時にframeLatencyWaitableObject_のカウンタを無理やり0にしたのでこの対応がいる。
	WaitForSingleObject(frameLatencyWaitableObject_, 1000);

	// max 60fps 固定
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	std::chrono::microseconds elapsed =
	    std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// 60ギリギリだとちょっとばかし高いリフレッシュレートのモニタで逆にかくついてしまうので少しバッファを取る
	static const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 62.0f));
	// 実際にwaitするのは60基準
	static const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
	std::chrono::microseconds check = kMinCheckTime - elapsed;
	if (std::chrono::microseconds(0) < check) {
		std::chrono::microseconds waitTime = kMinTime - elapsed;

		// sleepは信用ならないので1uでポーリング
		std::chrono::steady_clock::time_point waitStart = std::chrono::steady_clock::now();
		do {
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		} while (std::chrono::steady_clock::now() - waitStart < waitTime);
	}

	elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
	    std::chrono::steady_clock::now() - reference_);
	reference_ = std::chrono::steady_clock::now();

	commandAllocator_->Reset();
	commandList_->Reset(commandAllocator_.Get(), nullptr);
}

void DirectXCommon::ClearRenderTarget() {
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	// レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
	    rtvHeap_->GetCPUDescriptorHandleForHeapStart(), bbIndex,
	    device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	// 全画面クリア        Red   Green Blue  Alpha
	float clearColor[] = {0.1f, 0.25f, 0.5f, 0.0f}; // 青っぽい色
	commandList_->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
}

void DirectXCommon::ClearDepthBuffer() {
	// 深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH =
	    CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvHeap_->GetCPUDescriptorHandleForHeapStart());
	// 深度バッファのクリア
	commandList_->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

int32_t DirectXCommon::GetBackBufferWidth() const { return backBufferWidth_; }

int32_t DirectXCommon::GetBackBufferHeight() const { return backBufferHeight_; }

void DirectXCommon::SetRenderTargets(bool sRGB) {
	// バックバッファの番号を取得（2つなので0番か1番）
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();
	UINT rtvIndex = sRGB ? bbIndex : bbIndex + kLinearRTVStart;

	// レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(
	    rtvHeap_->GetCPUDescriptorHandleForHeapStart(), rtvIndex,
	    device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	// 深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH =
	    CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvHeap_->GetCPUDescriptorHandleForHeapStart());
	// レンダーターゲットをセット
	commandList_->OMSetRenderTargets(1, &rtvH, false, &dsvH);
}

void DirectXCommon::InitializeDXGIDevice([[maybe_unused]] bool enableDebugLayer) {
	HRESULT result = S_FALSE;

#ifdef _DEBUG
	if (enableDebugLayer) {
		ComPtr<ID3D12Debug> debugController;
		// デバッグレイヤーをオンに
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();
		}
		// DREDレポートをオンに
		ComPtr<ID3D12DeviceRemovedExtendedDataSettings> dredSettings;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dredSettings)))) {
			dredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
			dredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
		}
	}
#endif

	// 対応レベルの配列
	D3D_FEATURE_LEVEL levels[] = {
	    D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0,
	    D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
	};
	const char* featureLevelStrings[] = {
	    "12.2", "12.1", "12.0", "11.1", "11.0",
	};

	// DXGIファクトリーの生成
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(result));

	// アダプターの列挙用
	std::vector<ComPtr<IDXGIAdapter4>> adapters;
	// ここに特定の名前を持つアダプターオブジェクトが入る
	ComPtr<IDXGIAdapter4> tmpAdapter;
	// パフォーマンスが高いものから順に、全てのアダプターを列挙する
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(
	                     i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&tmpAdapter)) !=
	                 DXGI_ERROR_NOT_FOUND;
	     i++) {
		// 動的配列に追加する
		adapters.push_back(tmpAdapter);
	}

	// ハードウェアアダプタを優先的に処理
	std::stable_sort(
	    adapters.begin(), adapters.end(),
	    [](const ComPtr<IDXGIAdapter4>& lhs, const ComPtr<IDXGIAdapter4>& rhs) {
		    DXGI_ADAPTER_DESC3 lhsDesc;
		    lhs->GetDesc3(&lhsDesc); // アダプターの情報を取得
		    DXGI_ADAPTER_DESC3 rhsDesc;
		    rhs->GetDesc3(&rhsDesc); // アダプターの情報を取得
		    return (lhsDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) <
		           (rhsDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE);
	    });

	result = S_FALSE;
	D3D_FEATURE_LEVEL featureLevel;
	for (int i = 0; i < adapters.size(); i++) {
		// デバイスを生成
		for (int levelIndex = 0; levelIndex < _countof(levels); levelIndex++) {
			result =
			    D3D12CreateDevice(adapters[i].Get(), levels[levelIndex], IID_PPV_ARGS(&device_));
			if (SUCCEEDED(result)) {
				// デバイスを生成できた時点でループを抜ける
				featureLevel = levels[levelIndex];
				DebugText::GetInstance()->ConsolePrintf(
				    std::format("FeatureLevel : {}\n", featureLevelStrings[levelIndex]).c_str());
				break;
			}
		}

		// このアダプタで生成できてたら完了
		if (SUCCEEDED(result)) {
			break;
		}
	}

	assert(!!device_);
	assert(SUCCEEDED(result));

#ifdef _DEBUG
	if (enableDebugLayer) {
		ComPtr<ID3D12InfoQueue> infoQueue;
		if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
			// 抑制するエラー
			D3D12_MESSAGE_ID denyIds[] = {
			    /*
			     * Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			     * https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			     */
			    D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE};
			// 抑制する表示レベル
			D3D12_MESSAGE_SEVERITY severities[] = {D3D12_MESSAGE_SEVERITY_INFO};
			D3D12_INFO_QUEUE_FILTER filter{};
			filter.DenyList.NumIDs = _countof(denyIds);
			filter.DenyList.pIDList = denyIds;
			filter.DenyList.NumSeverities = _countof(severities);
			filter.DenyList.pSeverityList = severities;
			// 指定したエラーの表示を抑制する
			infoQueue->PushStorageFilter(&filter);
			// エラー時にブレークを発生させる
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		}
	}
#endif
}

void DirectXCommon::CreateSwapChain() {
	HRESULT result = S_FALSE;

	// 各種設定をしてスワップチェーンを生成
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = backBufferWidth_;
	swapChainDesc.Height = backBufferHeight_;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 色情報の書式を一般的なものに
	swapChainDesc.SampleDesc.Count = 1;                // マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; // バックバッファとして使えるように
	swapChainDesc.BufferCount = 2;                      // バッファ数を２つに設定
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // フリップ後は速やかに破棄
	swapChainDesc.Flags =
	    DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING |
	    DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT; // ティアリングサポート
	ComPtr<IDXGISwapChain1> swapChain1;
	result = dxgiFactory_->CreateSwapChainForHwnd(
	    commandQueue_.Get(), winApp_->GetHwnd(), &swapChainDesc, nullptr, nullptr, &swapChain1);
	assert(SUCCEEDED(result));

	// リフレッシュレートを取得。floatで取るのは大変なので大体あってれば良いので整数で。
	HDC hdc = GetDC(winApp_->GetHwnd());
	refreshRate_ = GetDeviceCaps(hdc, VREFRESH);
	ReleaseDC(winApp_->GetHwnd(), hdc);

	// SwapChain4を得る
	swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));
	assert(SUCCEEDED(result));

	// VSync共存型fps固定のためにレイテンシ1
	swapChain_->SetMaximumFrameLatency(1);

	// 実際のflip用イベントを取得
	frameLatencyWaitableObject_ = swapChain_->GetFrameLatencyWaitableObject();
	// 取得直後のカウンタが1になっているので、レイテンシ1にしても実際はバッファが1ある状態になるので、強制的に0にして即時flipを稼働させる。
	// たぶん非推奨ではある。
	WaitForSingleObject(frameLatencyWaitableObject_, INFINITE);

	// OSが行うAlt+Enterのフルスクリーンは制御不能なので禁止
	dxgiFactory_->MakeWindowAssociation(
	    winApp_->GetHwnd(), DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
}

void DirectXCommon::InitializeCommand() {
	HRESULT result = S_FALSE;

	// コマンドアロケータを生成
	result = device_->CreateCommandAllocator(
	    D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(result));

	// コマンドリストを生成
	result = device_->CreateCommandList(
	    0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr,
	    IID_PPV_ARGS(&commandList_));
	assert(SUCCEEDED(result));

	// 標準設定でコマンドキューを生成
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	result = device_->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(result));
}

void DirectXCommon::CreateFinalRenderTargets() {
	HRESULT result = S_FALSE;

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = swapChain_->GetDesc(&swcDesc);
	assert(SUCCEEDED(result));

	// 各種設定をしてディスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // レンダーターゲットビュー
	heapDesc.NumDescriptors = kNumRTVDescriptor;
	result = device_->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeap_));
	assert(SUCCEEDED(result));

	// 裏表の２つ分について
	backBuffers_.resize(swcDesc.BufferCount);
	for (int i = 0; i < backBuffers_.size(); i++) {
		// スワップチェーンからバッファを取得
		result = swapChain_->GetBuffer(i, IID_PPV_ARGS(&backBuffers_[i]));
		assert(SUCCEEDED(result));

		// ディスクリプタヒープのハンドルを取得
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		    rtvHeap_->GetCPUDescriptorHandleForHeapStart(), i,
		    device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		// レンダーターゲットビューの設定
		D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
		// シェーダーの計算結果をSRGBに変換して書き込む
		renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		renderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		// レンダーターゲットビューの生成
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &renderTargetViewDesc, handle);

		// Linear版のRTV作成
		handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
		    rtvHeap_->GetCPUDescriptorHandleForHeapStart(), i + kLinearRTVStart,
		    device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		renderTargetViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &renderTargetViewDesc, handle);
	}
}

void DirectXCommon::CreateDepthBuffer() {
	HRESULT result = S_FALSE;

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	// リソース設定
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
	    DXGI_FORMAT_D32_FLOAT, backBufferWidth_, backBufferHeight_, 1, 0, 1, 0,
	    D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	CD3DX12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);
	// リソースの生成
	result = device_->CreateCommittedResource(
	    &heapProps, D3D12_HEAP_FLAG_NONE, &depthResDesc,
	    D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値書き込みに使用
	    &clearValue, IID_PPV_ARGS(&depthBuffer_));
	assert(SUCCEEDED(result));

	// 深度ビュー用デスクリプタヒープ作成
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;                    // 深度ビューは1つ
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; // デプスステンシルビュー
	result = device_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap_));
	assert(SUCCEEDED(result));

	// 深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; // 深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device_->CreateDepthStencilView(
	    depthBuffer_.Get(), &dsvDesc, dsvHeap_->GetCPUDescriptorHandleForHeapStart());
}

void DirectXCommon::CreateFence() {
	HRESULT result = S_FALSE;

	// フェンスの生成
	result = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(result));
}
