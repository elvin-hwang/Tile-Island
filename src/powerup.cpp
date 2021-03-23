// Header
#include "powerup.hpp"
#include "render.hpp"
#include "physics.hpp"
#include "tile.hpp"
#include "blobule.hpp"
#include "utils.hpp"
#include <egg.hpp>
#include <iostream>

extern int current_turn;
extern int MAX_TURNS;

int NUM_POWERUPS = 3;
std::string BIG_POWERUP = "bigboi";
std::string SMALL_POWERUP = "smolboi";
std::string COLOR_SWAP_POWERUP = "colorswitcharoo";

void bigPowerup(ECS::Entity entity, PowerupSystem::Powerup& powerup)
{
    if (powerup.duration == -1)
    {
        powerup.duration = current_turn + 5;
        Motion& motion = ECS::registry<Motion>.get(entity);
        motion.scale *= vec2(1.5f, 1.5f);
    }

    else if (current_turn == powerup.duration)
    {
        Motion& motion = ECS::registry<Motion>.get(entity);
        motion.scale /= vec2(1.5f, 1.5f);
        ECS::registry<PowerupSystem::Powerup>.remove(entity);
    }
}

void smallPowerup(ECS::Entity entity, PowerupSystem::Powerup& powerup)
{
    if (powerup.duration == -1)
    {
        powerup.duration = current_turn + 5;
        Motion& motion = ECS::registry<Motion>.get(entity);
        motion.scale *= vec2(0.7f, 0.7f);
    }

    else if (current_turn == powerup.duration)
    {
        Motion& motion = ECS::registry<Motion>.get(entity);
        motion.scale /= vec2(0.7f, 0.7f);
        ECS::registry<PowerupSystem::Powerup>.remove(entity);
    }
}

void colorSwapPowerup(ECS::Entity entity)
{
    for (ECS::Entity entity : ECS::registry<Tile>.entities)
    {
        Tile::setRandomSplat(entity);
    }
    ECS::registry<PowerupSystem::Powerup>.remove(entity);
}

void PowerupSystem::handle_powerups()
{
	// Loop over all collisions detected by the physics system
	auto& registry = ECS::registry<PowerupSystem::Powerup>;
	for (auto& entity : ECS::registry<PowerupSystem::Powerup>.entities)
	{
		PowerupSystem::Powerup& powerup = ECS::registry<PowerupSystem::Powerup>.get(entity);

		// create random powerup
		if (powerup.power == BIG_POWERUP)
		{
            bigPowerup(entity, powerup);
		}

        if (powerup.power == SMALL_POWERUP)
        {
            smallPowerup(entity, powerup);
        }

        if (powerup.power == COLOR_SWAP_POWERUP)
        {
            colorSwapPowerup(entity);
        }
	}
}

void PowerupSystem::Powerup::createPowerup(ECS::Entity& entity)
{
    int curr_powerup = 0 + (std::rand() % (NUM_POWERUPS - 1 - 0 + 1));
	PowerupSystem::Powerup& powerup = ECS::registry<PowerupSystem::Powerup>.emplace(entity);
	powerup.owner = entity;
    switch (curr_powerup) {
        case 0:
            powerup.power = BIG_POWERUP;
            break;

        case 1:
            powerup.power = SMALL_POWERUP;
            break;

        default:
            powerup.power = COLOR_SWAP_POWERUP;
            break;
    }
}
