#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "world.hpp"

// Utils Functions
struct Utils
{
    // Get Active Player Blobule
    static ECS::Entity& getActivePlayerBlobule();

    // Get Euclidean distance between two motions
    static float euclideanDist(Motion motion1, Motion motion2);
};

