#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

// Utils Functions
struct Utils
{
    // Get Active Player Blobule
    static ECS::Entity& getActivePlayerBlobule();

    // Get Euclidean distance between two motions
    static float euclideanDist(Motion motion1, Motion motion2);

    static float getDist(float x1, float y1, float x2, float y2);

    static float getVelocityMagnitude(Motion motion);

    static bool circleIntersectsLine(vec2 center, float radius, float x1, float y1, float x2, float y2);

};

