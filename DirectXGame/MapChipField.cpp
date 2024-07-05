#include "MapChipField.h"
#include <algorithm> // std::clamp
#include <cassert>
#include <fstream>
#include <sstream>
#include <map>

namespace {
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};
}


uint32_t MapChipField::GetNumBlockVirtical() { return kNumBlockVirtical; }

uint32_t MapChipField::GetNumBlockHorizontal() { return kNumBlockHorizontal; }

void MapChipField::ResetMapChipData() {
	mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);

	for (auto& mapChipDataLine : mapChipData_.data) {
		mapChipDataLine.resize(kNumBlockHorizontal);
	}
}

void MapChipField::LoadMapChipCsv(const std::string& filePath) {
	// マップチップデータをリセット
	ResetMapChipData();

	// ファイルを開く
	std::ifstream file(filePath);
	assert(file.is_open());

	// マップチップCSVを読み込む
	std::stringstream mapChipCsv;
	mapChipCsv << file.rdbuf();
	file.close();

	// CSVからマップチップデータを解析して読み込む
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		std::string line;
		std::getline(mapChipCsv, line);
		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			std::string word;
			std::getline(line_stream, word, ',');

			if (mapChipTable.count(word) > 0) {
				mapChipData_.data[i][j] = mapChipTable[word];
			}
		}
	}
}

MapChipField::IndexSet MapChipField::GetMapChipIndexSetByPosition(const Vector3& position) {
	IndexSet indexSet = {};
	indexSet.xIndex = static_cast<uint32_t>(position.x / kBlockWidth);
	indexSet.yIndex = kNumBlockVirtical - 1 - static_cast<uint32_t>(position.y / kBlockHeight);

	// インデックスが範囲外の場合はクリップ
	indexSet.xIndex = std::clamp(indexSet.xIndex, 0u, kNumBlockHorizontal - 1);
	indexSet.yIndex = std::clamp(indexSet.yIndex, 0u, kNumBlockVirtical - 1);

	return indexSet;
}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) {
	if (xIndex >= 0 && xIndex < kNumBlockHorizontal && yIndex >= 0 && yIndex < kNumBlockVirtical) {
		return mapChipData_.data[yIndex][xIndex];
	}
	return MapChipType::kBlank;
}

Vector3 MapChipField::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex) {

	// Y座標を反転させる
	float x = xIndex * kBlockHeight + kBlockWidth / 2.0f;
	float y = (kNumBlockVirtical - 1 - yIndex) * kBlockHeight + kBlockHeight / 2.0f;
	float z = 0.0f;

	return Vector3(x, y, z);
}


MapChipField::Rect MapChipField::GetRectByIndex(uint32_t xIndex, uint32_t yIndex) {

	Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);

	Rect rect;
	rect.left = center.x - kBlockWidth / 2.0f;
	rect.right = center.x + kBlockWidth / 2.0f;
	rect.bottom = center.y - kBlockHeight / 2.0f;
	rect.top = center.y + kBlockHeight / 2.0f;

	return rect;
}
