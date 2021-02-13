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
    static ECS::Entity createTile(vec2 position, TerrainType type);
};

// All data relevant to the terrain of entities
struct Terrain {
    TerrainType type;
    float friction = 0.f;
};
