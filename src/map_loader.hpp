#pragma once

#include "common.hpp"
#include <string>
#include "tiny_ecs.hpp"

// tiles that form the island
struct MapLoader
{
	static std::vector<std::vector<ECS::Entity>> loadMap(std::string fileLocation, vec2 windowSize);

	static std::vector<std::vector<ECS::Entity>> loadSavedMap(vec2 windowSize);

	static void saveMap();

	static ECS::Entity getBlobule(int index);

	static std::vector<int> getTileGridLocation(ECS::Entity tile);
};

