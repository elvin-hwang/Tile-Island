#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

struct NPC
{
    // Create all the associated render resources and default transform.
    static ECS::Entity createNpc(vec2 position);

};

