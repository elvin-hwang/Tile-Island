// internal
#include "ai.hpp"
#include "tiny_ecs.hpp"
#include "blobule.hpp"
#include "utils.hpp"

#include <iostream>
#include <cstdlib>
#include <random>

float maxDistanceFromEgg = 150.f;

float eggSpeed = 75.f;

float responseDelay = 500.f;
float timer = 0.f;
float angle = 0.f;
int randomScope = 90.f; // The scope of possible random angles (in degrees)

void AISystem::step(float elapsed_ms, vec2 window_size_in_game_units)
{
	(void)elapsed_ms; // placeholder to silence unused warning until implemented
	(void)window_size_in_game_units; // placeholder to silence unused warning until implemented

	// egg ai here
	updateEggAiState();
	EggAiActOnState();
	timer -= elapsed_ms;

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
			timer = 0.f;
		}
		else if (eggAi.state == EggState::move)
		{
			if (timer <= 0.f) {
				angle = (rand() % randomScope + angle - randomScope / 2) * PI / 180;
				timer = responseDelay;
				eggMotion.velocity = { cos(angle) * eggSpeed, sin(angle) * eggSpeed };
			}
		}
	}
}

/*
Updates EggAi State based on various conditions.
All EggAi's will gain THE SAME randomized state for the duration of a players turn which lasts until the end of the players turn. 
Upon the next player's turn, the EggAi will get a new randomized state, which lasts for the duration of the active player's turn.
The EggAi will trigger that state whenever the active player's blobule moves within range of the EggAi's entity, and will return back to normal state when blobule moves out of range.
The EggAi ignores all other blobule that does not belong to the active player.
*/
void AISystem::updateEggAiState()
{
	// egg ai state updated here
	for (ECS::Entity& eggNPC : ECS::registry<EggAi>.entities)
	{
		Motion& eggMotion = ECS::registry<Motion>.get(eggNPC);
		EggAi& eggAi = ECS::registry<EggAi>.get(eggNPC);

		ECS::Entity& active_blobule = Utils::getActivePlayerBlobule();

		Motion& blobMotion = ECS::registry<Motion>.get(active_blobule);
		float dist = Utils::euclideanDist(eggMotion, blobMotion);

		if (dist < maxDistanceFromEgg)
		{
			angle = atan2(blobMotion.position.y - eggMotion.position.y, blobMotion.position.x - eggMotion.position.x) * 180 / PI + 180;
			std::string currentActivePlayer = ECS::registry<Blobule>.get(active_blobule).color;
			if (currentActivePlayer != lastActivePlayer)
			{
				lastActivePlayer = currentActivePlayer;
			}
			eggAi.state = EggState::move;
		}
		else {
			eggAi.state = EggState::normal;
		}
	}
}
