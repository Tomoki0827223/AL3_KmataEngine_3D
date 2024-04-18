#include "WinApp.h"

#include <string>

#ifdef _DEBUG
#include <imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT
    ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

namespace {

// ref: https://devblogs.microsoft.com/oldnewthing/20131017-00/?p=2903
BOOL UnadjustWindowRectEx(LPRECT prc, DWORD dwStyle, BOOL fMenu, DWORD dwExStyle) {
	RECT rc;
	SetRectEmpty(&rc);
	BOOL fRc = AdjustWindowRectEx(&rc, dwStyle, fMenu, dwExStyle);
	if (fRc) {
		prc->left -= rc.left;
		prc->top -= rc.top;
		prc->right -= rc.right;
		prc->bottom -= rc.bottom;
	}
	return fRc;
}
} // namespace

const wchar_t WinApp::kWindowClassName[] = L"DirectXGame";

WinApp* WinApp::GetInstance() {
	static WinApp instance;
	return &instance;
}

// ウィンドウプロシージャ
LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	WinApp* app = reinterpret_cast<WinApp*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
#ifdef _DEBUG
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
		return true;
#endif

	// メッセージで分岐
	switch (msg) {
	case WM_DESTROY:        // ウィンドウが破棄された
		PostQuitMessage(0); // OSに対して、アプリの終了を伝える
		return 0;

	case WM_SIZING: {
		// アスペクト比を変えるサイズ変更を許可しない
		if (app && app->GetSizeChangeMode() == WinApp::SizeChangeMode::kFixedAspect) {
			float aspectRatio = app->aspectRatio_;
			float aspectRatioRecp = 1.0f / aspectRatio;
			RECT* rect = reinterpret_cast<RECT*>(lparam);
			UnadjustWindowRectEx(
			    rect, GetWindowLong(hwnd, GWL_STYLE), GetMenu(hwnd) != 0,
			    GetWindowLong(hwnd, GWL_EXSTYLE));

			switch (wparam) {
			case WMSZ_LEFT:
			case WMSZ_BOTTOMLEFT:
			case WMSZ_RIGHT:
			case WMSZ_BOTTOMRIGHT:
				rect->bottom = rect->top + LONG((rect->right - rect->left) * aspectRatioRecp);
				break;
			case WMSZ_TOP:
			case WMSZ_TOPRIGHT:
			case WMSZ_BOTTOM:
				rect->right = rect->left + LONG((rect->bottom - rect->top) * aspectRatio);
				break;
			case WMSZ_TOPLEFT:
				rect->top = rect->bottom - LONG((rect->right - rect->left) * aspectRatioRecp);
				rect->left = rect->right - LONG((rect->bottom - rect->top) * aspectRatio);
				break;
			}

			AdjustWindowRectEx(
			    rect, GetWindowLong(hwnd, GWL_STYLE), GetMenu(hwnd) != 0,
			    GetWindowLong(hwnd, GWL_EXSTYLE));
		}
		break;
	}
	}
	return DefWindowProc(hwnd, msg, wparam, lparam); // 標準の処理を行う
}

void WinApp::CreateGameWindow(
    const wchar_t* title, UINT windowStyle, int32_t clientWidth, int32_t clientHeight) {

	// COM初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	windowStyle_ = windowStyle;
	aspectRatio_ = float(clientWidth) / float(clientHeight);
	// ウィンドウクラスの設定
	wndClass_.cbSize = sizeof(WNDCLASSEX);
	wndClass_.lpfnWndProc = (WNDPROC)WindowProc;     // ウィンドウプロシージャ
	wndClass_.lpszClassName = kWindowClassName;      // ウィンドウクラス名
	wndClass_.hInstance = GetModuleHandle(nullptr);  // ウィンドウハンドル
	wndClass_.hCursor = LoadCursor(NULL, IDC_ARROW); // カーソル指定

	RegisterClassEx(&wndClass_); // ウィンドウクラスをOSに登録

	// ウィンドウサイズ{ X座標 Y座標 横幅 縦幅 }
	RECT wrc = {0, 0, clientWidth, clientHeight};
	AdjustWindowRect(&wrc, windowStyle_, false); // 自動でサイズ補正

	// ウィンドウオブジェクトの生成
	hwnd_ = CreateWindow(
	    wndClass_.lpszClassName, // クラス名
	    title,                   // タイトルバーの文字
	    windowStyle_,            // タイトルバーと境界線があるウィンドウ
	    CW_USEDEFAULT,           // 表示X座標（OSに任せる）
	    CW_USEDEFAULT,           // 表示Y座標（OSに任せる）
	    wrc.right - wrc.left,    // ウィンドウ横幅
	    wrc.bottom - wrc.top,    // ウィンドウ縦幅
	    nullptr,                 // 親ウィンドウハンドル
	    nullptr,                 // メニューハンドル
	    wndClass_.hInstance,     // 呼び出しアプリケーションハンドル
	    nullptr);                // オプション
	SetWindowLongPtr(hwnd_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	// ウィンドウ表示
	ShowWindow(hwnd_, SW_NORMAL);
}

void WinApp::TerminateGameWindow() {
	// ウィンドウクラスを登録解除
	UnregisterClass(wndClass_.lpszClassName, wndClass_.hInstance);

	// COM 終了
	CoUninitialize();
}

bool WinApp::ProcessMessage() {
	MSG msg{}; // メッセージ

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // メッセージがある？
	{
		TranslateMessage(&msg); // キー入力メッセージの処理
		DispatchMessage(&msg);  // ウィンドウプロシージャにメッセージを送る
	}

	if (msg.message == WM_QUIT) // 終了メッセージが来たらループを抜ける
	{
		return true;
	}

	return false;
}

void WinApp::SetFullscreen(bool fullscreen) {

	if (isFullscreen_ != fullscreen) {
		if (fullscreen) {
			// 元の状態を覚えておく
			GetWindowRect(hwnd_, &windowRect_);

			// 仮想フルスクリーン化
			SetWindowLong(
			    hwnd_, GWL_STYLE,
			    windowStyle_ &
			        ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

			RECT fullscreenRect{0};
			HMONITOR monitor = MonitorFromWindow(hwnd_, MONITOR_DEFAULTTONEAREST);
			MONITORINFO info;
			info.cbSize = sizeof(info);
			GetMonitorInfo(monitor, &info);
			fullscreenRect.right = info.rcMonitor.right - info.rcMonitor.left;
			fullscreenRect.bottom = info.rcMonitor.bottom - info.rcMonitor.top;

			SetWindowPos(
			    hwnd_, HWND_TOPMOST, fullscreenRect.left, fullscreenRect.top, fullscreenRect.right,
			    fullscreenRect.bottom, SWP_FRAMECHANGED | SWP_NOACTIVATE);
			ShowWindow(hwnd_, SW_MAXIMIZE);

		} else {
			// 通常ウィンドウに戻す
			SetWindowLong(hwnd_, GWL_STYLE, windowStyle_);

			SetWindowPos(
			    hwnd_, HWND_NOTOPMOST, windowRect_.left, windowRect_.top,
			    windowRect_.right - windowRect_.left, windowRect_.bottom - windowRect_.top,
			    SWP_FRAMECHANGED | SWP_NOACTIVATE);

			ShowWindow(hwnd_, SW_NORMAL);
		}
	}

	isFullscreen_ = fullscreen;
}

bool WinApp::IsFullscreen() const { return isFullscreen_; }

void WinApp::SetSizeChangeMode(SizeChangeMode sizeChangeMode) {

	sizeChangeMode_ = sizeChangeMode;
	if (sizeChangeMode_ == SizeChangeMode::kNone) {
		windowStyle_ &= ~WS_THICKFRAME;
	} else {
		// アスペクト比変更不可なので現在のアスペクト比を持っておく
		if (sizeChangeMode_ == SizeChangeMode::kFixedAspect) {
			RECT clientRect{};
			GetClientRect(hwnd_, &clientRect);
			aspectRatio_ = float(clientRect.right - clientRect.left) /
			               float(clientRect.bottom - clientRect.top);
		}
		windowStyle_ |= WS_THICKFRAME;
	}
	SetWindowLong(hwnd_, GWL_STYLE, windowStyle_);
	SetWindowPos(
	    hwnd_, NULL, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED));
	ShowWindow(hwnd_, SW_NORMAL);
}

WinApp::SizeChangeMode WinApp::GetSizeChangeMode() const { return sizeChangeMode_; }
