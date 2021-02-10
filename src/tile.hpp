#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

enum TerrainType {
    Water,
    Ice,
    Mud
};

// tiles that form the island
struct Tile
{
    // Create all the associated render resources and default transform.
    static ECS::Entity createWaterTile(vec2 position);

    // Create all the associated render resources and default transform.
    static ECS::Entity createBlueTile(vec2 position);
    
    // Create all the associated render resources and default transform.
    static ECS::Entity createPurpleTile(vec2 position);
};

// All data relevant to the terrain of entities
struct Terrain {
    TerrainType type;
    float friction = 0.f;
};
