#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

// tiles that form the island
struct Tile
{
    // Create all the associated render resources and default transform.
    static ECS::Entity createBlueTile(vec2 position);
    
    // Create all the associated render resources and default transform.
    static ECS::Entity createPurpleTile(vec2 position);
};
