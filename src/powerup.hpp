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
        static void createPowerup(ECS::Entity& entity);
        std::string power = "none";
        int duration = 0;
    };

private:
};

