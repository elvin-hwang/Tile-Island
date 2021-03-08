#pragma once

#include "common.hpp"
#include "subject.hpp"
#include "tiny_ecs.hpp"

class CollisionSystem
{
public: 
    void initialize_collisions();
    void handle_collisions();

private:
    Subject blobule_tile_coll;
    Subject blobule_blobule_coll;
    Subject blobule_egg_coll;
    Subject egg_tile_coll;
};

 