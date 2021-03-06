// Header

#include "map_loader.hpp"
#include "csv_reader.hpp"
#include "common.hpp"
#include "json.hpp"
#include "tile.hpp"
#include "blobule.hpp"
#include "egg.hpp"
#include "utils.hpp"
#include <istream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

int widthNum;
int heightNum;

int initialPlayer;
int initialRound;

// Blobules are always in order of yellow, green, red, blue
std::vector<ECS::Entity> blobuleList;
std::vector<std::vector<ECS::Entity>> tileIsland;
std::string loadedGridLocation;

const std::string savedGridLocation = "data/saved/grid.csv";
const std::string savedMapLocation = "data/saved/map.json";

void createTileIsland(std::vector<std::vector<std::string>> csvGrid) {
	tileIsland.clear();
	for (int i = 0; i < csvGrid.size(); i++) {
		auto row = csvGrid[i];
		std::vector<ECS::Entity> newRow;
		for (int j = 0; j < row.size(); j++) {
			ECS::Entity tile;
			std::string value = row[j];
			value.erase(std::remove(value.begin(), value.end(), '\r'), value.end());

			float xPos = tileSize * (j + 1);
			float yPos = tileSize * (i + 1);

			if (value == "Block") {
				tile = Tile::createTile({ xPos, yPos }, Block);
			}
			else if (value == "Ice") {
				tile = Tile::createTile({ xPos, yPos }, Ice);
			}
			else if (value == "Mud") {
				tile = Tile::createTile({ xPos, yPos }, Mud);
			}
			else if (value == "Sand") {
				tile = Tile::createTile({ xPos, yPos }, Sand);
			}
			else if (value == "Acid") {
				tile = Tile::createTile({ xPos, yPos }, Acid);
			}
			else if (value == "Speed") {
				tile = Tile::createTile({ xPos, yPos }, Speed);
			}
			else if (value == "SpeedLeft") {
				tile = Tile::createTile({ xPos, yPos }, Speed_LEFT);
			}
			else if (value == "SpeedRight") {
				tile = Tile::createTile({ xPos, yPos }, Speed_RIGHT);
			}
			else if (value == "SpeedUp") {
				tile = Tile::createTile({ xPos, yPos }, Speed_UP);
			}
			else if (value == "SpeedDown") {
				tile = Tile::createTile({ xPos, yPos }, Speed_DOWN);
			}
			else if (value == "Teleport") {
				tile = Tile::createTile({ xPos, yPos }, Teleport);
			}
			else { // default case: value == "Water"
				tile = Tile::createTile({ xPos, yPos }, Water);
			}
			auto& tileComponent = ECS::registry<Tile>.get(tile);
			tileComponent.gridLocation = { j, i };
			newRow.push_back(tile);
		}
		tileIsland.push_back(newRow);
	}
}

void createBlobules(std::vector<std::vector<int>> blobulePositions) {
	blobuleList.clear();
	int count = 0;
	for (auto position : blobulePositions) {
		auto& motion = ECS::registry<Motion>.get(tileIsland[position[1]][position[0]]);
		ECS::Entity blob;
		switch (count) {
		case 0:
			blob = Blobule::createBlobule(motion.position, blobuleCol::Yellow, "yellow");
			break;
		case 1:
			blob = Blobule::createBlobule(motion.position, blobuleCol::Green, "green");
			break;
		case 2:
			blob = Blobule::createBlobule(motion.position, blobuleCol::Red, "red");
			break;
		case 3:
			blob = Blobule::createBlobule(motion.position, blobuleCol::Blue, "blue");
			break;
		}
		ECS::registry<Blobule>.get(blob).currentGrid = position;
		blobuleList.push_back(blob);
		count++;
	}
}

void createEggs(std::vector<std::vector<int>> eggPositions) {
	for (auto position : eggPositions) {
		auto& motion = ECS::registry<Motion>.get(tileIsland[position[1]][position[0]]);
		Egg::createEgg(motion.position);
	}
}

void createWaterBorder(vec2 windowSize) {
	float waterBorderWidth = 700.f;

	vec2 topLeft = ECS::registry<Motion>.get(tileIsland[0][0]).position;
	vec2 bottomRight = ECS::registry<Motion>.get(tileIsland[heightNum - 1][widthNum - 1]).position;

	float top = topLeft.y - tileSize;
	float bot = bottomRight.y + tileSize;
	float left = topLeft.x - tileSize;
	float right = bottomRight.x + tileSize;

	float rightBound = windowSize.x + waterBorderWidth;
	float bottomBound = windowSize.y + waterBorderWidth;
	float leftBound = -waterBorderWidth;
	float topBound = -waterBorderWidth;

	// Top Water Border
	for (float y = top; y >= topBound; y -= tileSize) {
		for (float x = rightBound; x >= leftBound; x -= tileSize) {
			Tile::createTile({ x, y }, Water);
		}
	}

	// Bottom Water Border
	for (float y = bot; y <= bottomBound; y += tileSize) {
		for (float x = rightBound; x >= leftBound; x -= tileSize) {
			Tile::createTile({ x, y }, Water);
		}
	}

	// Left Water Border
	for (float x = left; x >= leftBound; x -= tileSize) {
		for (float y = topLeft.y; y < bot; y += tileSize) {
			Tile::createTile({ x, y }, Water);
		}
	}

	// Right Water Border
	for (float x = right; x <= rightBound; x += tileSize) {
		for (float y = topLeft.y; y < bot; y += tileSize) {
			Tile::createTile({ x, y }, Water);
		}
	}
}

void centerIsland(vec2 windowSize) {
	vec2 offset = vec2{ windowSize.x / 2, windowSize.y / 2 } - ECS::registry<Motion>.get(tileIsland[heightNum / 2][widthNum / 2]).position;
	Utils::moveCamera(offset.x, offset.y);
}

void setSplatPositions(nlohmann::json mapInfo) {
	// Set splat positions
	std::vector<std::vector<int>> yellowSplats = mapInfo["yellowSplat"];
	std::vector<std::vector<int>> greenSplats = mapInfo["greenSplat"];
	std::vector<std::vector<int>> redSplats = mapInfo["redSplat"];
	std::vector<std::vector<int>> blueSplats = mapInfo["blueSplat"];

	for (auto gridLocation : yellowSplats) {
		Tile::setSplat(tileIsland[gridLocation[1]][gridLocation[0]], blobuleCol::Yellow);
	}
	for (auto gridLocation : greenSplats) {
		Tile::setSplat(tileIsland[gridLocation[1]][gridLocation[0]], blobuleCol::Green);
	}
	for (auto gridLocation : redSplats) {
		Tile::setSplat(tileIsland[gridLocation[1]][gridLocation[0]], blobuleCol::Red);
	}
	for (auto gridLocation : blueSplats) {
		Tile::setSplat(tileIsland[gridLocation[1]][gridLocation[0]], blobuleCol::Blue);
	}
}

void setBlobuleScales(std::vector<std::vector<float>> blobuleScales) {
	for (int i = 0; i < blobuleScales.size(); i++) {
		auto entity = blobuleList[i];
		auto scale = blobuleScales[i];
		auto& motion = ECS::registry<Motion>.get(entity);
		motion.scale = { scale[0], scale[1] };
	}
}


// PUBLIC functions
std::vector<std::vector<ECS::Entity>> MapLoader::loadMap(std::string fileLocation, vec2 windowSize) {
	nlohmann::json mapInfo;
	std::ifstream map_file(fileLocation, std::ifstream::binary);
	map_file >> mapInfo;

	initialPlayer = mapInfo["currentPlayer"];
	initialRound = mapInfo["currentRound"];
	widthNum = mapInfo["numWidth"];
	heightNum = mapInfo["numHeight"];

	loadedGridLocation = mapInfo["gridInfo"];
	std::ifstream temp(loadedGridLocation);
	std::istream& file = temp;
	std::vector<std::vector<std::string>> csvGrid = CSVReader::readCSV(file);

	int gridHeight = csvGrid.size();
	int gridWidth = csvGrid[0].size();
	if (gridWidth != widthNum || gridHeight != heightNum) {
		throw "INVALID HEIGHT AND WIDTH IN FILES";
	}

	createTileIsland(csvGrid);
	createBlobules(mapInfo["blobulePositions"]);
	createEggs(mapInfo["eggPositions"]);
	centerIsland(windowSize);
	createWaterBorder(windowSize);

	// Relevant only to saved maps
	setSplatPositions(mapInfo);
	setBlobuleScales(mapInfo["blobuleScales"]);

	return tileIsland;
}

void MapLoader::saveMap(int currentPlayer, int currentTurn) {
	if (loadedGridLocation.empty()) {
		return;
	}

	if (loadedGridLocation != savedGridLocation) {
		std::ifstream  src(loadedGridLocation, std::ios::binary);
		std::ofstream  dst(savedGridLocation, std::ios::binary);

		dst << src.rdbuf();
	}
	nlohmann::json mapInfo;
	std::ifstream readFile(savedMapLocation, std::ios::binary);
	readFile >> mapInfo;

	std::ofstream writeFile(savedMapLocation, std::ios::binary);

	// Save grid info
	mapInfo["gridInfo"] = savedGridLocation;
	mapInfo["numWidth"] = widthNum;
	mapInfo["numHeight"] = heightNum;

	// Save current info
	mapInfo["currentPlayer"] = currentPlayer;
	mapInfo["currentRound"] = currentTurn;

	// SAVE BLOBULE INFO
	std::vector<std::vector<int>> entitiesPosition;
	std::vector<std::vector<float>> entitiesScale;
	for (auto entity : blobuleList) {
		auto& blob = ECS::registry<Blobule>.get(entity);
		auto& motion = ECS::registry<Motion>.get(entity);
		entitiesPosition.push_back(blob.currentGrid);
		entitiesScale.push_back({ motion.scale.x, motion.scale.y });
	}
	mapInfo["blobulePositions"] = entitiesPosition;
	mapInfo["blobuleScales"] = entitiesScale;

	// SAVE EGG INFO
	std::vector<std::vector<int>> eggsPosition;
	for (ECS::Entity entity : ECS::registry<Egg>.entities) {
		auto& egg = ECS::registry<Egg>.get(entity);
		eggsPosition.push_back(egg.gridLocation);
	}
	mapInfo["eggPositions"] = eggsPosition;

	// SAVE SPLAT INFO
	std::vector<std::vector<int>> yellowSplats;
	std::vector<std::vector<int>> greenSplats;
	std::vector<std::vector<int>> redSplats;
	std::vector<std::vector<int>> blueSplats;

	for (int i = 0; i < tileIsland.size(); i++) {
		auto row = tileIsland[i];
		for (int j = 0; j < row.size(); j++) {
			auto& tile = ECS::registry<Tile>.get(tileIsland[i][j]);
			std::vector<int> currentGrid = { j, i };
			if (ECS::registry<YellowSplat>.has(tile.splatEntity)) {
				yellowSplats.push_back(currentGrid);
			}
			else if (ECS::registry<GreenSplat>.has(tile.splatEntity)) {
				greenSplats.push_back(currentGrid);
			}
			else if (ECS::registry<RedSplat>.has(tile.splatEntity)) {
				redSplats.push_back(currentGrid);
			}
			else if (ECS::registry<BlueSplat>.has(tile.splatEntity)) {
				blueSplats.push_back(currentGrid);
			}
		}
	}
	mapInfo["yellowSplat"] = yellowSplats;
	mapInfo["greenSplat"] = greenSplats;
	mapInfo["redSplat"] = redSplats;
	mapInfo["blueSplat"] = blueSplats;

	writeFile << mapInfo;
}

std::vector<int> MapLoader::getTileGridLocation(ECS::Entity tile) {
	for (int i = 0; i < tileIsland.size(); i++) {
		auto row = tileIsland[i];
		for (int j = 0; j < row.size(); j++) {
			if (tileIsland[i][j].id == tile.id) {
				return { j, i };
			}
		}
	}

	return { -1, -1 };
}

ECS::Entity MapLoader::getBlobule(int index) {
	return blobuleList[index];
}

std::vector<int> MapLoader::getInitialInfo() {
	return { initialPlayer, initialRound };
}
