#pragma once
#include <Windows.h>
#include <cstdint>

/// <summary>
/// ウィンドウズアプリケーション
/// </summary>
class WinApp {
public: // 静的メンバ変数
	// ウィンドウサイズ
	static const int kWindowWidth = 1280; // 横幅
	static const int kWindowHeight = 720; // 縦幅
	// ウィンドウクラス名
	static const wchar_t kWindowClassName[];

	enum class SizeChangeMode {
		kNone,        //!< サイズ変更不可
		kNormal,      //!< 自由変更
		kFixedAspect, //!< アスペクト比一定
	};

public: // 静的メンバ関数
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static WinApp* GetInstance();

	/// <summary>
	/// ウィンドウプロシージャ
	/// </summary>
	/// <param name="hwnd">ウィンドウハンドル</param>
	/// <param name="msg">メッセージ番号</param>
	/// <param name="wparam">メッセージ情報1</param>
	/// <param name="lparam">メッセージ情報2</param>
	/// <returns>成否</returns>
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public: // メンバ関数
	/// <summary>
	/// ゲームウィンドウの作成
	/// <param name="title">ウィンドウタイトル</param>
	/// <param name="windowStyle">ウィンドウの初期スタイル</param>
	/// <param name="clientWidth">ウィンドウのクライアント領域の初期幅</param>
	/// <param name="clientHeight">ウィンドウのクライアント領域の初期高さ</param>
	/// </summary>
	void CreateGameWindow(
	    const wchar_t* title = L"DirectXGame", UINT windowStyle = WS_OVERLAPPEDWINDOW,
	    int32_t clientWidth = kWindowWidth, int32_t clientHeight = kWindowHeight);

	/// <summary>
	/// ゲームウィンドウの破棄
	/// </summary>
	void TerminateGameWindow();

	/// <summary>
	/// メッセージの処理
	/// </summary>
	/// <returns>終了かどうか</returns>
	bool ProcessMessage();

	/// <summary>
	/// ウィンドウハンドルの取得
	/// </summary>
	/// <returns></returns>
	HWND GetHwnd() const { return hwnd_; }

	HINSTANCE GetHInstance() const { return wndClass_.hInstance; }

	/// <summary>
	/// フルスクリーン設定
	/// </summary>
	/// <param name="fullscreen">フルスクリーンにするかどうか</param>
	void SetFullscreen(bool fullscreen);

	/// <summary>
	/// フルスクリーンかどうか
	/// </summary>
	/// <returns></returns>
	bool IsFullscreen() const;

	/// <summary>
	/// サイズ変更モードの設定
	/// </summary>
	/// <returns></returns>
	void SetSizeChangeMode(SizeChangeMode sizeChangeMode);

	/// <summary>
	/// サイズ変更モードの取得
	/// </summary>
	/// <returns></returns>
	SizeChangeMode GetSizeChangeMode() const;

private: // メンバ関数
	WinApp() = default;
	~WinApp() = default;
	WinApp(const WinApp&) = delete;
	const WinApp& operator=(const WinApp&) = delete;

private: // メンバ変数
	// Window関連
	HWND hwnd_ = nullptr;   // ウィンドウハンドル
	WNDCLASSEX wndClass_{}; // ウィンドウクラス
	UINT windowStyle_;
	bool isFullscreen_ = false;
	RECT windowRect_;
	SizeChangeMode sizeChangeMode_ = SizeChangeMode::kNormal;
	float aspectRatio_;
};
