#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

// main menu start screen
struct Menu
{
    static ECS::Entity createMenu(vec2 position);
};
