#pragma once

#include "common.hpp"
#include <string>
#include "tiny_ecs.hpp"

// tiles that form the island
struct MapLoader
{
	static std::vector<std::vector<ECS::Entity>> loadMap(std::string fileLocation, vec2 windowSize);

	static void loadSavedMap();

	static void saveMap();

	static ECS::Entity getBlobule(int index);
};

