// internal
#include "ai.hpp"
#include "tiny_ecs.hpp"
#include "blobule.hpp"

#include <iostream>
#include <cstdlib>
#include <random>

float maxDistanceFromEgg = 150.f;

float eggSpeed = 50.f;

void AISystem::step(float elapsed_ms, vec2 window_size_in_game_units)
{
	(void)elapsed_ms; // placeholder to silence unused warning until implemented
	(void)window_size_in_game_units; // placeholder to silence unused warning until implemented

	// egg ai here
	updateEggAiState();
	EggAiActOnState();
	// add other ai steps...
}

/*
* EggAi performs action based on whatever state is it set to.
*/
void AISystem::EggAiActOnState()
{
	for (ECS::Entity& eggNPC : ECS::registry<EggAi>.entities)
	{
		Motion& eggMotion = ECS::registry<Motion>.get(eggNPC);
		EggAi& eggAi = ECS::registry<EggAi>.get(eggNPC);

		if (eggAi.state == EggState::normal)
		{
			eggMotion.velocity = { 0, 0 };
		}
		else if (eggAi.state == EggState::panic)
		{
			// this guard so so that the egg doesnt keep moving when next to nonmoving blobules
			if (eggAi.velOfNearbyBlobule != vec2(0.f, 0.f))
			{
				float angle = atan2(eggAi.velOfNearbyBlobule.y, eggAi.velOfNearbyBlobule.x);
				eggMotion.velocity = { cos(angle) * eggSpeed, sin(angle) * eggSpeed };
			}
			else {
				eggMotion.velocity = { 0.f, 0.f };
			}
			eggAi.state = EggState::normal;
		}
	}
}

/*
Updates EggAi State based on various conditions
*/
void AISystem::updateEggAiState()
{
	// egg ai state updated here
	for (ECS::Entity& eggNPC : ECS::registry<EggAi>.entities)
	{
		Motion& eggMotion = ECS::registry<Motion>.get(eggNPC);
		EggAi& eggAi = ECS::registry<EggAi>.get(eggNPC);

		ECS::Entity& active_blobule = getActivePlayerBlobule();
		Motion& blobMotion = ECS::registry<Motion>.get(active_blobule);
		float dist = euclideanDist(eggMotion, blobMotion);

		// eggAi will be set to panic if it a blob is within distance
		if (dist < maxDistanceFromEgg)
		{
			eggAi.state = EggState::panic;
			eggAi.velOfNearbyBlobule = blobMotion.velocity;
		}
	}
}

ECS::Entity& AISystem::getActivePlayerBlobule()
{
	for (ECS::Entity& blobule : ECS::registry<Blobule>.entities)
	{
		if (ECS::registry<Blobule>.get(blobule).active_player)
		{
			return blobule;
		}
	}
	throw "no active player set";
}

float AISystem::euclideanDist(Motion motion1, Motion motion2)
{
	float x = motion1.position.x - motion2.position.x;
	float y = motion1.position.y - motion2.position.y;

	float dist = pow(x, 2) + pow(y, 2);
	return sqrt(dist);
}



