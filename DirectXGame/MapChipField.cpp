#include "MapChipField.h"
#include <map>
#include <fstream>
#include <sstream>

void MapChipField::ResetMapChipdata(){

    mapChipData_.data.clear();
	mapChipData_.data.resize(kNumBlockVirtical);
	for (std::vector<MapChipType>& mapChipDataLine : mapChipData_.data) {
	
		mapChipDataLine.resize(kNumBlockHorizontal);
	}

}

void MapChipField::LordMapChipCsv(const std::string& filePath){

	ResetMapChipdata();

	std::ifstream file;
	file.open(filePath);
	assert(file.is_open());

	std::stringstream mapChipCsv;

	mapChipCsv << file.rdbuf();
	file.close();

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
	
		std::string line;
		getline(mapChipCsv, line);

		std::istringstream line_stream(line);

		for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
		
			std::string world;
			getline(line_stream, world, ',');

			if (mapchipTable.contains(world)) {
				mapChipData_.data[i][j] = mapchipTable[world];
			}
		}
	}
}

MapChipType MapChipField::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex) { 
	
	if (xIndex < 0 || kNumBlockHorizontal - 1 < xIndex) {
		return MapChipType::kBlank;
	}
	if (yIndex < 0 || kNumBlockVirtical - 1 < yIndex) {
		return MapChipType::kBlank;
	}

	return mapChipData_.data[yIndex][xIndex];
}

Vector3 MapChipField::GetMapChipostionByIndex(uint32_t xIndex, uint32_t yIndex) { 
	return Vector3(kBlockWidth * xIndex, kBlockHeigth * (kNumBlockVirtical - 1 - yIndex), 0); };


std::map<std::string, MapChipType> mapchipTable = {

	{"0",MapChipType::kBlank},
    {"1",MapChipType::kBlock},

};

namespace {

std::map<std::string, MapChipType> mapChipTable = {

    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};

}