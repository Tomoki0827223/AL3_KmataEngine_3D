#pragma once

#include "Sprite.h"
#include <Windows.h>
#include <string>

/// <summary>
/// デバッグ用文字表示
/// </summary>
class DebugText {
public:
	// デバッグテキスト用のテクスチャ番号を指定
	static const int kMaxCharCount = 1024; // 最大文字数
	static const int kFontWidth = 9;       // フォント画像内1文字分の横幅
	static const int kFontHeight = 18;     // フォント画像内1文字分の縦幅
	static const int kFontLineCount = 14;  // フォント画像内1行分の文字数
	static const int kBufferSize = 512;    // 書式付き文字列展開用バッファサイズ

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static DebugText* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 文字列追加
	/// </summary>
	/// <param name="text">文字列</param>
	/// <param name="x">表示座標X</param>
	/// <param name="y">表示座標Y</param>
	/// <param name="scale">倍率</param>
	void Print(const std::string& text, float x, float y, float scale = 1.0f);

	/// <summary>
	/// 書式付き文字列追加
	/// </summary>
	/// <param name="fmt">書式付き文字列</param>
	void Printf(const char* fmt, ...);

	/// <summary>
	/// 書式付きコンソール出力
	/// </summary>
	/// <param name="fmt">書式付き文字列</param>
	void ConsolePrintf(const char* fmt, ...);

	/// <summary>
	/// 描画フラッシュ
	/// </summary>
	void DrawAll();

	/// <summary>
	/// 描画座標の指定
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	void SetPos(float x, float y) {
		posX_ = x;
		posY_ = y;
	}

	/// <summary>
	/// 描画倍率の指定
	/// </summary>
	/// <param name="scale">倍率</param>
	void SetScale(float scale) { scale_ = scale; }

private:
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
	// スプライトデータの配列
	Sprite* spriteDatas_[kMaxCharCount] = {};
	// スプライトデータ配列の添え字番号
	int spriteIndex_ = 0;

	float posX_ = 0.0f;
	float posY_ = 0.0f;
	float scale_ = 1.0f;
	// 書式付き文字列展開用バッファ
	char buffer[kBufferSize];

	DebugText();
	~DebugText();
	DebugText(const DebugText&) = delete;
	DebugText& operator=(const DebugText&) = delete;
	void NPrint(int len, const char* text);
};
