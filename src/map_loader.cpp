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

int widthNum;
int heightNum;

// Blobules are always in order of yellow, green, red, blue
std::vector<ECS::Entity> blobuleList;
std::vector<std::vector<ECS::Entity>> tileIsland;
std::string loadedGridLocation;

const std::string savedGridLocation = "../../../data/saved/grid.csv";
const std::string savedMapLocation = "../../../data/saved/map.json";
const float tile_width = 44.46f;

void createTileIsland(std::vector<std::vector<std::string>> csvGrid) {
	tileIsland.clear();
	for (int i = 0; i < csvGrid.size(); i++) {
		auto row = csvGrid[i];
		std::vector<ECS::Entity> newRow;
		for (int j = 0; j < row.size(); j++) {
			ECS::Entity tile;
			auto value = row[j];

			float xPos = tile_width * (j + 1);
			float yPos = tile_width * (i + 1);

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

	float top = topLeft.y - tile_width;
	float bot = bottomRight.y + tile_width;
	float left = topLeft.x - tile_width;
	float right = bottomRight.x + tile_width;

	float rightBound = windowSize.x + waterBorderWidth;
	float bottomBound = windowSize.y + waterBorderWidth;
	float leftBound = -waterBorderWidth;
	float topBound = -waterBorderWidth;

	// Top Water Border
	for (float y = top; y >= topBound; y -= tile_width) {
		for (float x = rightBound; x >= leftBound; x -= tile_width) {
			Tile::createTile({ x, y }, Water);
		}
	}

	// Bottom Water Border
	for (float y = bot; y <= bottomBound; y += tile_width) {
		for (float x = rightBound; x >= leftBound; x -= tile_width) {
			Tile::createTile({ x, y }, Water);
		}
	}

	// Left Water Border
	for (float x = left; x >= leftBound; x -= tile_width) {
		for (float y = topLeft.y; y <= bottomRight.y; y += tile_width) {
			Tile::createTile({ x, y }, Water);
		}
	}

	// Right Water Border
	for (float x = right; x <= rightBound; x += tile_width) {
		for (float y = topLeft.y; y <= bottomRight.y; y += tile_width) {
			Tile::createTile({ x, y }, Water);
		}
	}
}

void centerIsland(vec2 windowSize) {
	auto& motion = ECS::registry<Motion>.get(tileIsland[heightNum / 2][widthNum / 2]);
	vec2 offset = vec2{ windowSize.x / 2, windowSize.y / 2 } - motion.position;
	Utils::moveCamera(offset.x, offset.y);
}


// PUBLIC functions
std::vector<std::vector<ECS::Entity>> MapLoader::loadMap(std::string fileLocation, vec2 windowSize) {
	nlohmann::json mapInfo;
	std::ifstream map_file(fileLocation, std::ifstream::binary);
	map_file >> mapInfo;

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
	return tileIsland;
}

std::vector<std::vector<ECS::Entity>> MapLoader::loadSavedMap(vec2 windowSize) {
	loadedGridLocation = savedGridLocation;
	tileIsland = MapLoader::loadMap(savedMapLocation, windowSize);


	nlohmann::json mapInfo;
	std::ifstream map_file(savedMapLocation, std::ifstream::binary);
	map_file >> mapInfo;

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

	return tileIsland;
}

void MapLoader::saveMap() {
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

	// SAVE BLOBULE INFO
	std::vector<std::vector<int>> entitiesPosition;
	for (auto entity : blobuleList) {
		auto& blob = ECS::registry<Blobule>.get(entity);
		entitiesPosition.push_back(blob.currentGrid);
	}
	mapInfo["blobulePositions"] = entitiesPosition;


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




//// TODO DELETE HERE TO...
//// Make a 20 x 15 Grid of Tiles.
//numWidth = (window_width - borderWidth * 2) / tile_width + 1;
//numHeight = (window_height - borderWidth * 2) / tile_width;
//
//int horizontalIndex = 0;
//int verticalIndex = 0;
//bool isTile = false;
//
//// Horizontally...
//for (int i = tile_width / 2 - borderWidth * 5; i <= window_width + borderWidth * 5; i += tile_width)
//{
//    // Vertically...
//    for (int j = tile_width / 2 - borderWidth * 5; j <= window_height + borderWidth * 5; j += tile_width)
//    {
//        if (i < borderWidth || j < borderWidth || i > window_width - borderWidth || j > window_height - borderWidth) {
//            Tile::createTile({ i, j }, Water);
//            continue;
//        }
//        islandGrid[horizontalIndex][verticalIndex] = { i, j };
//        // Generate map
//        if ((horizontalIndex < numWidth - 2 && horizontalIndex > 2) && (verticalIndex == 0 || verticalIndex == numHeight)) {
//            Tile::createTile({ i, j }, Block); // top, bottom wall
//        }
//        else if ((verticalIndex < numHeight - 2 && verticalIndex > 2) && (horizontalIndex == 0 || horizontalIndex == numWidth)) {
//            Tile::createTile({ i, j }, Block); // left, right wall
//        }
//        else if (i < window_width / 2) {
//            if (i == 281.f && j == 237.f) {
//                Tile::createTile({ i, j }, Speed);
//            }
//            else if (i == 325.f && j == 545.f) {
//                Tile::createTile({ i, j }, Teleport);
//            }
//            else if (i == 149.f && j == 413.f) {
//                Tile::createTile({ i, j }, Speed_RIGHT);
//            }
//            else if (i == 457.f && j == 149.f) {
//                Tile::createTile({ i, j }, Speed_DOWN);
//            }
//            else {
//                Tile::createTile({ i, j }, Ice);
//            }
//        }
//        else {
//            if (i == 633.f && j == 457.f) {
//                Tile::createTile({ i, j }, Speed);
//                // std::cout << "(" << i << ", " << j << ")";
//            }
//            else if (i == 721.f && j == 325.f) {
//                Tile::createTile({ i, j }, Teleport);
//                // std::cout << "(" << i << ", " << j << ")";
//            }
//            else if (i == 853.f && j == 369.f) {
//                Tile::createTile({ i, j }, Speed_LEFT);
//                // std::cout << "(" << i << ", " << j << ")";
//            }
//            else if (i == 501.f && j == 633.f) {
//                Tile::createTile({ i, j }, Speed_UP);
//                // std::cout << "(" << i << ", " << j << ")";
//            }
//            else {
//                Tile::createTile({ i, j }, Mud);
//                // std::cout << "(" << i << ", " << j << ")";
//            }
//        }
//        verticalIndex++;
//        isTile = true;
//    }
//    if (isTile) {
//        verticalIndex = 0;
//        horizontalIndex++;
//        isTile = false;
//    }
//}
//
//// Create blobule characters
//if (ECS::registry<Blobule>.components.size() <= 4) {
//    player_blobule1 = Blobule::createBlobule({ islandGrid[1][1].x, islandGrid[1][1].y }, blobuleCol::Yellow, "yellow");
//    player_blobule2 = Blobule::createBlobule({ islandGrid[numWidth - 1][1].x, islandGrid[numWidth - 1][1].y }, blobuleCol::Green,
//        "green");
//    player_blobule3 = Blobule::createBlobule({ islandGrid[1][numHeight - 1].x, islandGrid[1][numHeight - 1].y }, blobuleCol::Red, "red");
//    player_blobule4 = Blobule::createBlobule({ islandGrid[numWidth - 1][numHeight - 1].x , islandGrid[numWidth - 1][numHeight - 1].y },
//        blobuleCol::Blue, "blue");
//    active_player = player_blobule1;
//    ECS::registry<Blobule>.get(active_player).active_player = true;
//}
//
////Only one npc for now
//if (ECS::registry<Egg>.components.size() < 1) {
//    // Create egg
//    ECS::Entity entity = Egg::createEgg({ islandGrid[numWidth / 2][numHeight / 2].x, islandGrid[numWidth / 2][numHeight / 2].y });
//    //add movement things here
//}
