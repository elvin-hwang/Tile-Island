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

    static float getDist(vec2 lineStart, vec2 lineEnd);

    static float getVelocityMagnitude(Motion motion);

    static bool circleIntersectsLine(vec2 center, float radius, vec2 lineStart, vec2 lineEnd);

    static vec2 Utils::getPerpendicularPoint(vec2 center, vec2 lineStart, vec2 lineEnd);

};

