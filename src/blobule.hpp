#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

enum class blobuleCol {
    Red,
    Blue,
    Green,
    Yellow
};

// Blobule characters in game
struct Blobule
{
    vec2 origin;

    // Create all the associated render resources and default transform.
    static ECS::Entity createBlobule(vec2 position, blobuleCol col, std::string colString);

};

