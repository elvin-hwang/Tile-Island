#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

// tiles that form the island
struct Menu
{
    static ECS::Entity createMenu(vec2 position);
};
