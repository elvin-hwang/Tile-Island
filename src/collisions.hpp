#pragma once

#include "common.hpp"
#include "subject.hpp"
#include "tiny_ecs.hpp"
#include "world.hpp"

class CollisionSystem
{
public: 
    void initialize_collisions();
    void handle_collisions();
    void resetTileCount();
    
    unsigned int yellow_tiles = 0;
    unsigned int green_tiles = 0;
    unsigned int red_tiles = 0;
    unsigned int blue_tiles = 0;
    
private:
    Subject blobule_tile_coll;
    Subject blobule_blobule_coll;
};

