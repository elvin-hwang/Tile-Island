#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

struct Egg
{
    // Create all the associated render resources and default transform.
    static ECS::Entity createEgg(vec2 position);
    
    static void reloadEgg(vec2 position);
};

