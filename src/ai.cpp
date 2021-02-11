// internal
#include "ai.hpp"
#include "tiny_ecs.hpp"
#include "blobule.hpp"

#include <iostream>
#include <cstdlib>
#include <random>

float maxDistanceFromEgg = 150.f;

float eggSpeed = 75.f;

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
		else if (eggAi.state == EggState::moveUp)
		{
			eggMotion.velocity = { 0, -eggSpeed };
		}
		else if (eggAi.state == EggState::moveDown)
		{
			eggMotion.velocity = { 0, eggSpeed };
		}
		else if (eggAi.state == EggState::moveLeft)
		{
			eggMotion.velocity = { -eggSpeed, 0 };
		}
		else if (eggAi.state == EggState::moveRight)
		{
			eggMotion.velocity = { eggSpeed, 0 };
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

		ECS::Entity& active_blobule = getActivePlayerBlobule();

		Motion& blobMotion = ECS::registry<Motion>.get(active_blobule);
		float dist = euclideanDist(eggMotion, blobMotion);

		if (dist < maxDistanceFromEgg)
		{
			std::string currentActivePlayer = ECS::registry<Blobule>.get(active_blobule).color;
			if (currentActivePlayer != lastActivePlayer)
			{
				lastActivePlayer = currentActivePlayer;
				currentState = rand() % 4;

			}
			switch (currentState)
			{
			case 0:
				eggAi.state = EggState::moveDown;
				break;
			case 1:
				eggAi.state = EggState::moveLeft;
				break;
			case 2:
				eggAi.state = EggState::moveRight;
				break;
			case 3:
				eggAi.state = EggState::moveUp;
				break;
			}
		}
		else {
			eggAi.state = EggState::normal;
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

	float dist = (float)pow(x, 2) + (float)pow(y, 2);
	return sqrt(dist);
}
