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

	// egg ai state updated here
	eggAiStep();

	// add other ai steps...
}

void AISystem::eggAiStep()
{
	// egg ai state updated here
	for (ECS::Entity& eggNPC : ECS::registry<EggAi>.entities)
	{
		Motion& eggMotion = ECS::registry<Motion>.get(eggNPC);
		EggAi& eggAi = ECS::registry<EggAi>.get(eggNPC);

		// timer is so that an egg which state was recently changed cannot be changed again immediately
		eggAi.timer--;

		for (ECS::Entity& blobule : ECS::registry<Blobule>.entities)
		{
			if (eggAi.timer <= 0)
			{
				Motion& blobMotion = ECS::registry<Motion>.get(blobule);
				float dist = euclideanDist(eggMotion, blobMotion);

				// Egg Panic State
				if (dist < maxDistanceFromEgg)
				{
					eggAi.state = EggState::panic;
					eggAi.timer = 50;
					float angle = atan2(blobMotion.velocity.y, blobMotion.velocity.x);

					// this guard so so that the egg doesnt keep moving when next to nonmoving blobules
					if (angle != 0)
					{
						eggMotion.velocity = { cos(angle) * eggSpeed, sin(angle) * eggSpeed };
					}
				}

				// Egg Normal State 
				else
				{
					eggAi.state = EggState::normal;
					eggMotion.velocity = { 0, 0 };
				}
			}
		}

	}
}

float AISystem::euclideanDist(Motion motion1, Motion motion2)
{
	float x = motion1.position.x - motion2.position.x;
	float y = motion1.position.y - motion2.position.y;

	float dist = pow(x, 2) + pow(y, 2);
	return sqrt(dist);
}



