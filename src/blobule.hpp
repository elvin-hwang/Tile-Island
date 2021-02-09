#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

// tiles that form the island
struct Blobule
{
    // Create all the associated render resources and default transform.
    static ECS::Entity createBlobule(vec2 position);
};

