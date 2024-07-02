#pragma once
#include "Vector3.h" // 通常はダブルクオートで囲まれます
#include <algorithm> // std::clampのために必要
#include <string>
#include <vector>

enum class MapChipType {
	kBlank,
	kBlock,
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

class MapChipField {
public:
	struct Rect {
		float left;
		float right;
		float bottom;
		float top;
	};

	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);
	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);

	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
	uint32_t GetNumBlockVirtical();
	uint32_t GetNumBlockHorizontal();

private:
	MapChipData mapChipData_;
};
