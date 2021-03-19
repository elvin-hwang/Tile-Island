#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "blobule.hpp"
#include <vector>

enum TerrainType {
    Water,
    Block,
    Ice,
    Mud,
    Speed,
    Speed_UP,
    Speed_LEFT,
    Speed_RIGHT,
    Speed_DOWN,
    Teleport
};

// tiles that form the island
struct Tile
{
    ECS::Entity splatEntity;

    // Create all the associated render resources and default transform.
    static ECS::Entity createTile(vec2 position, TerrainType type);

    static void setSplat(ECS::Entity entity, blobuleCol color);

    static void setRandomSplat(ECS::Entity entity);
};

// All data relevant to the terrain of entities
struct Terrain {
    TerrainType type;
    float friction = 0.f;
    // std::vector<vec2> teleport_positions;
};

// All data relevant to teleportation tiles
struct Teleporting {
    vec2 position = { 0, 0 };
};
