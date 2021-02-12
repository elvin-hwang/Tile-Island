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
    Subject blobule_wall_coll;
    Subject blobule_tile_coll;
};

