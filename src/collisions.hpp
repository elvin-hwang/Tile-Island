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
    static Subject& blobule_wall_coll;
    static Subject& blobule_tile_coll;
};

