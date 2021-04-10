#include "common.hpp"
#include "world.hpp"
#include "json.hpp"
#include "blobule.hpp"
#include "egg.hpp"
#include "level_editor.hpp"
#include "map_loader.hpp"
#include "tile.hpp"
#include "text.hpp"

// stlib
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;
std::vector<ECS::Entity> editor_blobule_list = {};
std::vector<ECS::Entity> editor_egg_list = {};
ECS::Entity notification_text;
bool has_changes = false;
int numTeleporters = 0;

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

// Do not let blobules be placed on block tiles or water tiles
bool bad_blobule_placement(ECS::Entity entity) {
	Tile tile_component = ECS::registry<Tile>.get(entity);
	return (tile_component.terrain_type == TerrainType::Block || tile_component.terrain_type == TerrainType::Water);
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

void LevelEditor::clear_entity_lists() {
	editor_blobule_list.clear();
	editor_egg_list.clear();
	numTeleporters = 0;
}

void LevelEditor::add_blobule(ECS::Entity blobule) {
	editor_blobule_list.push_back(blobule);
}

// Place an entity on the specified grid coordinate
// Will replace existing tiles/existing blobs
void LevelEditor::place_entity(std::vector<std::vector<ECS::Entity>>& grid, EditorEntity entity, vec2 grid_coords) {
	// Mark the map as changed and new
	has_changes = true;

	// Remove any text that is not the save button text
	for (ECS::Entity text : ECS::registry<Text>.entities)
	{
		if (ECS::registry<Text>.get(text).content != "Save")
		{
			ECS::registry<Text>.remove(text);
		}
	}

	// Find the tile we're placing on
	int x_coord = grid_coords.x;
	int y_coord = grid_coords.y;
	ECS::Entity selected_tile = grid[y_coord][x_coord];
	Motion& selected_tile_motion = ECS::registry<Motion>.get(selected_tile);

	// If the entity that's being placed is a tile, replace the selected tile
	if (is_tile(entity))
	{
		// Do not place block/water tiles or other blobules on tiles with existing blobules
		if (entity == EditorEntity::Block || entity == EditorEntity::Water)
		{
			for (ECS::Entity blob : editor_blobule_list)
			{
				if (ECS::registry<Blobule>.get(blob).currentGrid[0] == x_coord && ECS::registry<Blobule>.get(blob).currentGrid[1] == y_coord)
				{
					return;
				}
			}
		}

		if (entity == EditorEntity::Teleport)
			numTeleporters++;

		ECS::registry<Tile>.remove(selected_tile);
		ECS::Entity tile = Tile::createTile(selected_tile_motion.position, entity_to_terrain_type(entity));
		grid[y_coord][x_coord] = tile;
		auto& tile_component = ECS::registry<Tile>.get(tile);
		tile_component.gridLocation = { y_coord, x_coord };
	}
	// If the entity that is being placed is a blob, move it appropriately
	else if (entity == EditorEntity::YellowBlob)
	{
		// Check for water/block tiles
		if (bad_blobule_placement(selected_tile))
			return;

		// Check that we're not trying to take another blob's spot
		for (ECS::Entity blob : editor_blobule_list)
		{
			if (ECS::registry<Blobule>.get(blob).color != "yellow")
			{
				if (ECS::registry<Blobule>.get(blob).currentGrid[0] == x_coord && ECS::registry<Blobule>.get(blob).currentGrid[1] == y_coord)
					return;
			}
		}

		// Find the right blob and move it
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
		// Check for water/block tiles
		if (bad_blobule_placement(selected_tile))
			return;

		// Check that we're not trying to take another blob's spot
		for (ECS::Entity blob : editor_blobule_list)
		{
			if (ECS::registry<Blobule>.get(blob).color != "green")
			{
				if (ECS::registry<Blobule>.get(blob).currentGrid[0] == x_coord && ECS::registry<Blobule>.get(blob).currentGrid[1] == y_coord)
					return;
			}
		}

		// Find the right blob and move it
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
		// Check for water/block tiles
		if (bad_blobule_placement(selected_tile))
			return;

		// Check that we're not trying to take another blob's spot
		for (ECS::Entity blob : editor_blobule_list)
		{
			if (ECS::registry<Blobule>.get(blob).color != "red")
			{
				if (ECS::registry<Blobule>.get(blob).currentGrid[0] == x_coord && ECS::registry<Blobule>.get(blob).currentGrid[1] == y_coord)
					return;
			}
		}

		// Find the right blob and move it
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
		// Check for water/block tiles
		if (bad_blobule_placement(selected_tile))
			return;

		// Check that we're not trying to take another blob's spot
		for (ECS::Entity blob : editor_blobule_list)
		{
			if (ECS::registry<Blobule>.get(blob).color != "blue")
			{
				if (ECS::registry<Blobule>.get(blob).currentGrid[0] == x_coord && ECS::registry<Blobule>.get(blob).currentGrid[1] == y_coord)
					return;
			}
		}

		// Find the right blob and move it
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
		// Only allow one egg
		if (editor_egg_list.size() < 1) {
			ECS::Entity egg = Egg::createEgg(selected_tile_motion.position);
			ECS::registry<Egg>.get(egg).gridLocation = { x_coord, y_coord };
			editor_egg_list.push_back(egg);
		}
	}
}

// Save the map that is on the screen to the '\data\level' folder
void LevelEditor::save_map(std::vector<std::vector<ECS::Entity>> grid) {
	// Checks how many maps we already have
	int numMaps = std::distance(fs::directory_iterator("data/level/"), fs::directory_iterator()) / 2 - 1;

	// Don't save a new save if nothing has changed or we've hit map limit
	if (!has_changes)
	{
		Text::create_text("No changes yet", { 300.f, 40.f }, 0.5);
		return;
	}
	else if (numMaps >= 6)
	{
		Text::create_text("Max maps reached", { 300.f, 40.f }, 0.5);
		return;
	}
	else if (numTeleporters == 1)
	{
		Text::create_text("Need more than one teleporter", { 300.f, 40.f }, 0.5);
		return;
	}

	// Save CSV
	std::string grid_name = "grid_" + std::to_string(numMaps + 1) + ".csv";
	std::string map_info_name = "map_" + std::to_string(numMaps + 1) + ".json";
	fs::path grid_path = fs::current_path() / "data" / "level" / grid_name;
	fs::path map_info_path = fs::current_path() / "data" / "level" / map_info_name;

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

	// All changes have been saved
	Text::create_text("Save Complete", { 300.f, 40.f }, 0.5);
	has_changes = false;
}
