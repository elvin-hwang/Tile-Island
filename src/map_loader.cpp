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

const float tile_width = 44.46f;
std::vector<ECS::Entity> blobuleList;

std::vector<std::vector<ECS::Entity>> createTileIsland(std::vector<std::vector<std::string>> csvGrid) {
	std::vector<std::vector<ECS::Entity>> tileIsland;
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
	return tileIsland;
}

void createBlobules(std::vector<std::vector<int>> blobulePositions, std::vector<std::vector<ECS::Entity>> tileIsland) {
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

void createEggs(std::vector<std::vector<int>> eggPositions, std::vector<std::vector<ECS::Entity>> tileIsland) {
	for (auto position : eggPositions) {
		auto& motion = ECS::registry<Motion>.get(tileIsland[position[1]][position[0]]);
		Egg::createEgg(motion.position);
	}
}

void createWaterBorder(std::vector<std::vector<ECS::Entity>> tileIsland, vec2 windowSize) {
	float top = ECS::registry<Motion>.get(tileIsland[0][0]).position.y - tile_width;
	float rightBound = windowSize.x + 700;
	float bottomBound = windowSize.y + 700;
	float leftBound = -700;
	float topBound = -700;

	for (float y = top; y >= topBound; y -= tile_width) {
		for (float x = rightBound; x >= leftBound; x -= tile_width) {
			Tile::createTile({ x, y }, Water);
		}
	}
	// while x = 1500 and y = top - tileWIDTH
	// while > -500 
}

void centerIsland(std::vector<std::vector<ECS::Entity>> tileIsland, vec2 windowSize) {
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

	std::string gridInfoPath = mapInfo["gridInfo"];
	std::ifstream temp(gridInfoPath);
	std::istream& file = temp;
	std::vector<std::vector<std::string>> csvGrid = CSVReader::readCSV(file);

	int gridHeight = csvGrid.size();
	int gridWidth = csvGrid[0].size();
	if (gridWidth != widthNum || gridHeight != heightNum) {
		throw "INVALID HEIGHT AND WIDTH IN FILES";
	}

	std::vector<std::vector<ECS::Entity>> tileIsland = createTileIsland(csvGrid);
	createBlobules(mapInfo["blobulePositions"], tileIsland);
	createEggs(mapInfo["eggPositions"], tileIsland);
	centerIsland(tileIsland, windowSize);
	createWaterBorder(tileIsland, windowSize);
	return tileIsland;
}

void MapLoader::loadSavedMap() {
	// TODO
}

void MapLoader::saveMap() {
	// TODO
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
