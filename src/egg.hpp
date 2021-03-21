#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

struct Egg
{
    std::vector<int> gridLocation = { -1, -1 };
    // Create all the associated render resources and default transform.
    static ECS::Entity createEgg(vec2 position);

};

