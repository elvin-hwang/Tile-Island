#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

enum blobuleCol {
    Red,
    Blue,
    Green,
    Yellow
};

// tiles that form the island
struct Blobule
{
    // Create all the associated render resources and default transform.
    static ECS::Entity createBlobule(vec2 position, blobuleCol col);

};

