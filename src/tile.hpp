#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

enum TerrainType {
    Water,
    Block,
    Ice,
    Mud
};

// tiles that form the island
struct Tile
{
    // Create all the associated render resources and default transform.
    static ECS::Entity createTile(vec2 position, TerrainType type, std::string blobule_color);
    
    static void reloadTile(vec2 position, TerrainType type, std::string blobule_color);
};

// All data relevant to the terrain of entities
struct Terrain {
    TerrainType type;
    float friction = 0.f;
    vec2 position = { 0, 0 };
    std::string key;
};
