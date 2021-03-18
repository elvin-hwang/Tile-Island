#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

class PowerupSystem
{
public:
    void initialize_powerups();
    void handle_powerups();

    // Stucture to store collision information
    struct Powerup
    {
        ECS::Entity owner;     
        static void createPowerup(ECS::Entity& other);
        std::string power = "none";
        int duration = 0;
    };

private:
    //Subject blobule_tile_coll;
    //Subject blobule_blobule_coll;
    //Subject blobule_egg_coll;
    //Subject egg_tile_coll;

    //// Music References
    //Mix_Chunk* collision_sound;
    //Mix_Chunk* splash_sound;
    //Mix_Chunk* powerup_sound;
};

