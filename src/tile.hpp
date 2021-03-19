#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "blobule.hpp"

enum TerrainType {
    Water,
    Block,
    Ice,
    Mud,
    Speed,
    Teleport
};

// tiles that form the island
struct Tile
{
    ECS::Entity splatEntity;

    // Create all the associated render resources and default transform.
    static ECS::Entity createTile(vec2 position, TerrainType type);

    static void setSplat(ECS::Entity entity, blobuleCol color);
};

// All data relevant to the terrain of entities
struct Terrain {
    TerrainType type;
    float friction = 0.f;
};
