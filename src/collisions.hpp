#pragma once

#include "common.hpp"
#include "subject.hpp"
#include "tiny_ecs.hpp"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

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
    
    // Music References
    Mix_Chunk* collision_sound;
    Mix_Chunk* splash_sound;
};

 
