#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

struct Wall
{
    // Create all the associated render resources and default transform.
    static ECS::Entity createWall(std::string wall_type, vec2 position, float angle);
};
