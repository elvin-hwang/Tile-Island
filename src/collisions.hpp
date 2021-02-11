#pragma once

#include "common.hpp"
#include "subject.hpp"
#include "tiny_ecs.hpp"

class Collisions
{
public: 
    static void Collisions::initialize_collisions();
    void Collisions::handle_collisions();

private:
    Subject blobule_wall_coll;
    Subject blobule_tile_coll;
};

