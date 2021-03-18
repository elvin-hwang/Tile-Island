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
	for (auto& powerup : ECS::registry<PowerupSystem::Powerup>.components)
	{
		std::cout << powerup.power << std::endl;
		// create random powerup


	}
	// Remove all collisions from this simulation step
	ECS::registry<PowerupSystem::Powerup>.clear();
}

void PowerupSystem::Powerup::createPowerup(ECS::Entity& entity)
{
	PowerupSystem::Powerup powerup = ECS::registry<PowerupSystem::Powerup>.emplace(entity);
	powerup.owner = entity;
	powerup.power = "randomPowerup";
	powerup.duration = 1;
}
