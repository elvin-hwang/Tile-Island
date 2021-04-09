#include "common.hpp"
#include "world.hpp"
#include "json.hpp"
#include "blobule.hpp"
#include "egg.hpp"
#include "level_editor.hpp"
#include "map_loader.hpp"
#include "tile.hpp"

// stlib
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;
std::vector<ECS::Entity> editor_blobule_list = {};
std::vector<ECS::Entity> editor_egg_list = {};

bool is_tile(LevelEditor::EditorEntity entity) {
	return (
		entity == LevelEditor::EditorEntity::Water ||
		entity == LevelEditor::EditorEntity::Block ||
		entity == LevelEditor::EditorEntity::Ice ||
		entity == LevelEditor::EditorEntity::Mud ||
		entity == LevelEditor::EditorEntity::Sand ||
		entity == LevelEditor::EditorEntity::Acid ||
		entity == LevelEditor::EditorEntity::Speed ||
		entity == LevelEditor::EditorEntity::Speed_UP ||
		entity == LevelEditor::EditorEntity::Speed_LEFT ||
		entity == LevelEditor::EditorEntity::Speed_RIGHT ||
		entity == LevelEditor::EditorEntity::Speed_DOWN ||
		entity == LevelEditor::EditorEntity::Teleport
		);
}

TerrainType entity_to_terrain_type(LevelEditor::EditorEntity entity) {
	TerrainType terrain_type;
	switch (entity) {
	case LevelEditor::EditorEntity::Water:
		terrain_type = TerrainType::Water;
		break;
	case LevelEditor::EditorEntity::Block:
		terrain_type = TerrainType::Block;
		break;
	case LevelEditor::EditorEntity::Ice:
		terrain_type = TerrainType::Ice;
		break;
	case LevelEditor::EditorEntity::Mud:
		terrain_type = TerrainType::Mud;
		break;
	case LevelEditor::EditorEntity::Sand:
		terrain_type = TerrainType::Sand;
		break;
	case LevelEditor::EditorEntity::Acid:
		terrain_type = TerrainType::Acid;
		break;
	case LevelEditor::EditorEntity::Speed:
		terrain_type = TerrainType::Speed;
		break;
	case LevelEditor::EditorEntity::Speed_UP:
		terrain_type = TerrainType::Speed_UP;
		break;
	case LevelEditor::EditorEntity::Speed_LEFT:
		terrain_type = TerrainType::Speed_LEFT;
		break;
	case LevelEditor::EditorEntity::Speed_RIGHT:
		terrain_type = TerrainType::Speed_RIGHT;
		break;
	case LevelEditor::EditorEntity::Speed_DOWN:
		terrain_type = TerrainType::Speed_DOWN;
		break;
	case LevelEditor::EditorEntity::Teleport:
		terrain_type = TerrainType::Teleport;
		break;
	default:
		terrain_type = TerrainType::Ice;
	}
	return terrain_type;
}

std::string tile_to_CSV(Tile tile) {
	std::string CSV_value;
	switch (tile.terrain_type) {
	case TerrainType::Water:
		CSV_value = "Water";
		break;
	case TerrainType::Block:
		CSV_value = "Block";
		break;
	case TerrainType::Ice:
		CSV_value = "Ice";
		break;
	case TerrainType::Mud:
		CSV_value = "Mud";
		break;
	case TerrainType::Sand:
		CSV_value = "Sand";
		break;
	case TerrainType::Acid:
		CSV_value = "Acid";
		break;
	case TerrainType::Speed:
		CSV_value = "Speed";
		break;
	case TerrainType::Speed_UP:
		CSV_value = "SpeedUp";
		break;
	case TerrainType::Speed_LEFT:
		CSV_value = "SpeedLeft";
		break;
	case TerrainType::Speed_RIGHT:
		CSV_value = "SpeedRight";
		break;
	case TerrainType::Speed_DOWN:
		CSV_value = "SpeedDown";
		break;
	case TerrainType::Teleport:
		CSV_value = "Teleport";
		break;
	default:
		CSV_value = "Ice";
	}
	return CSV_value;
}

void LevelEditor::add_blobule(ECS::Entity blobule) {
	editor_blobule_list.push_back(blobule);
}

// Place an entity on the specified grid coordinate
// Will replace existing tiles/existing blobs
void LevelEditor::place_entity(std::vector<std::vector<ECS::Entity>>& grid, EditorEntity entity, vec2 grid_coords) {
	// Find the tile we're placing on
	int x_coord = grid_coords.x;
	int y_coord = grid_coords.y;
	ECS::Entity selected_tile = grid[y_coord][x_coord];
	Motion& selected_tile_motion = ECS::registry<Motion>.get(selected_tile);

	// If the entity that's being placed is a tile, replace the selected tile
	if (is_tile(entity))
	{
		ECS::registry<Tile>.remove(selected_tile);
		ECS::Entity tile = Tile::createTile(selected_tile_motion.position, entity_to_terrain_type(entity));
		grid[y_coord][x_coord] = tile;
		auto& tile_component = ECS::registry<Tile>.get(tile);
		tile_component.gridLocation = { y_coord, x_coord };
	}
	// If the entity that is being placed is a blob, remove the blob it's replacing
	else if (entity == EditorEntity::YellowBlob)
	{
		for (ECS::Entity blob : editor_blobule_list)
		{
			if (ECS::registry<Blobule>.get(blob).color == "yellow")
			{
				ECS::registry<Motion>.get(blob).position = selected_tile_motion.position;
				ECS::registry<Blobule>.get(blob).currentGrid = { x_coord, y_coord };
			}
		}
	}
	else if (entity == EditorEntity::GreenBlob)
	{
		for (ECS::Entity blob : editor_blobule_list)
		{
			if (ECS::registry<Blobule>.get(blob).color == "green")
			{
				ECS::registry<Motion>.get(blob).position = selected_tile_motion.position;
				ECS::registry<Blobule>.get(blob).currentGrid = { x_coord, y_coord };
			}
		}
	}
	else if (entity == EditorEntity::RedBlob)
	{
		for (ECS::Entity blob : editor_blobule_list)
		{
			if (ECS::registry<Blobule>.get(blob).color == "red")
			{
				ECS::registry<Motion>.get(blob).position = selected_tile_motion.position;
				ECS::registry<Blobule>.get(blob).currentGrid = { x_coord, y_coord };
			}
		}
	}
	else if (entity == EditorEntity::BlueBlob)
	{
		for (ECS::Entity blob : editor_blobule_list)
		{
			if (ECS::registry<Blobule>.get(blob).color == "blue")
			{
				ECS::registry<Motion>.get(blob).position = selected_tile_motion.position;
				ECS::registry<Blobule>.get(blob).currentGrid = { x_coord, y_coord };
			}
		}
	}
	else if (entity == EditorEntity::Egg)
	{
		ECS::Entity egg = Egg::createEgg(selected_tile_motion.position);
		ECS::registry<Egg>.get(egg).gridLocation = { x_coord, y_coord };
		editor_egg_list.push_back(egg);
	}
}

// Save the map that is on the screen to the '\data\level' folder
void LevelEditor::save_map(std::vector<std::vector<ECS::Entity>> grid) {
	// Run checks to see if all requirements are fulfilled

	// Save CSV
	fs::path grid_path = fs::current_path() / "data" / "level" / "grid_3.csv";
	fs::path map_info_path = fs::current_path() / "data" / "level" / "map_3.json";

	std::ofstream gridFile(grid_path, std::ios::binary);

	int numRows = grid.size();
	int numCols = grid[0].size();

	// Create CSV by creating rows iteratively
	for (int i = 0; i < numRows; i++)
	{
		for (int j = 0; j < numCols; j++)
		{
			Tile current_tile = ECS::registry<Tile>.get(grid[i][j]);
			gridFile << tile_to_CSV(current_tile) + (j < numCols - 1 ? "," : "\n");
		}
	}

	// Save JSON file (copied over from map_loader.cpp)
	nlohmann::json mapInfo;
	std::ofstream mapInfoFile(map_info_path, std::ios::binary);

	// Save grid info
	mapInfo["gridInfo"] = grid_path.string();
	mapInfo["numWidth"] = numCols;
	mapInfo["numHeight"] = numRows;

	// Save current info
	mapInfo["currentPlayer"] = 0;
	mapInfo["currentRound"] = 0;

	// SAVE BLOBULE INFO
	std::vector<std::vector<int>> entitiesPosition;
	std::vector<std::vector<float>> entitiesScale;
	for (auto entity : editor_blobule_list) {
		auto& blob = ECS::registry<Blobule>.get(entity);
		auto& motion = ECS::registry<Motion>.get(entity);
		entitiesPosition.push_back(blob.currentGrid);
		entitiesScale.push_back({ motion.scale.x, motion.scale.y });
	}
	mapInfo["blobulePositions"] = entitiesPosition;
	mapInfo["blobuleScales"] = entitiesScale;

	// SAVE EGG INFO
	std::vector<std::vector<int>> eggsPosition;
	for (ECS::Entity entity : editor_egg_list) {
		auto& egg = ECS::registry<Egg>.get(entity);
		eggsPosition.push_back(egg.gridLocation);
	}
	mapInfo["eggPositions"] = eggsPosition;

	// Not saving splat info in level editor
	std::vector<std::vector<int>> yellowSplats;
	std::vector<std::vector<int>> greenSplats;
	std::vector<std::vector<int>> redSplats;
	std::vector<std::vector<int>> blueSplats;

	mapInfo["yellowSplat"] = yellowSplats;
	mapInfo["greenSplat"] = greenSplats;
	mapInfo["redSplat"] = redSplats;
	mapInfo["blueSplat"] = blueSplats;

	mapInfoFile << mapInfo;
}
