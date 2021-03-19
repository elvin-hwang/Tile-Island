// Header
#include "powerup.hpp"
#include "render.hpp"
#include "physics.hpp"
#include "tile.hpp"
#include "blobule.hpp"
#include "utils.hpp"
#include <egg.hpp>
#include <iostream>

void PowerupSystem::initialize_powerups() {

};

void PowerupSystem::handle_powerups()
{
	// Loop over all collisions detected by the physics system
	auto& registry = ECS::registry<PowerupSystem::Powerup>;
	for (auto& entity : ECS::registry<PowerupSystem::Powerup>.entities)
	{
		PowerupSystem::Powerup& powerup = ECS::registry<PowerupSystem::Powerup>.get(entity);
		std::cout << powerup.power << std::endl;
		// create random powerup
		if (powerup.power == "bigboi")
		{
			std::cout << powerup.power << std::endl;
			powerup.duration = 1;
			Motion& motion = ECS::registry<Motion>.get(entity);
			motion.scale *= vec2(2.f, 2.f);
		}

	}
	// Remove all collisions from this simulation step
	ECS::registry<PowerupSystem::Powerup>.clear();
}

void PowerupSystem::Powerup::createPowerup(ECS::Entity& entity)
{
	PowerupSystem::Powerup& powerup = ECS::registry<PowerupSystem::Powerup>.emplace(entity);
	powerup.owner = entity;
	powerup.power = "bigboi";
	powerup.duration = 1;
}
