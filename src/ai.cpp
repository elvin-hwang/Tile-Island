// internal
#include "ai.hpp"
#include "tiny_ecs.hpp"
#include "blobule.hpp"

#include <iostream>
#include <cstdlib>
#include <random>

float maxDistanceFromEgg = 200.f;

std::random_device rd;  //Will be used to obtain a seed for the random number engine
std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
std::uniform_int_distribution<int> distrib(-100, 100);


void AISystem::step(float elapsed_ms, vec2 window_size_in_game_units)
{
	(void)elapsed_ms; // placeholder to silence unused warning until implemented
	(void)window_size_in_game_units; // placeholder to silence unused warning until implemented

	// egg ai state updated here
	updateState();

	for (ECS::Entity& eggNPC : ECS::registry<EggAi>.entities)
	{
		Motion& eggMotion = ECS::registry<Motion>.get(eggNPC);

		if (ECS::registry<EggAi>.get(eggNPC).state == EggState::normal)
		{
			eggMotion.velocity = vec2(0.f, 0.f);
		}

		if (ECS::registry<EggAi>.get(eggNPC).state == EggState::panic)
		{
			/*float x = (float) distrib(gen);
			float y = (float) distrib(gen);
			std::cout << x << std::endl;
			std::cout << x << std::endl;*/
			eggMotion.velocity = vec2(100.f, 100.f);
		}
	}
}

void AISystem::updateState()
{
	// egg ai state updated here
	for (ECS::Entity& eggNPC : ECS::registry<EggAi>.entities)
	{
		Motion& eggMotion = ECS::registry<Motion>.get(eggNPC);
		EggAi& eggAi = ECS::registry<EggAi>.get(eggNPC);

		eggAi.timer--;

		for (ECS::Entity& blobule : ECS::registry<Blobule>.entities)
		{
			if (eggAi.timer <= 0)
			{
				Motion blobMotion = ECS::registry<Motion>.get(blobule);

				float dist = euclideanDist(eggMotion, blobMotion);

				if (dist < maxDistanceFromEgg)
				{
					eggAi.state = EggState::panic;
					eggAi.timer = 100;
				}
				else
				{
					eggAi.state = EggState::normal;
				}
			}
		}

	}


	// add other states...
}

float AISystem::euclideanDist(Motion motion1, Motion motion2)
{
	float x = motion1.position.x - motion2.position.x;
	float y = motion1.position.y - motion2.position.y;

	float dist = pow(x, 2) + pow(y, 2);
	return sqrt(dist);
}



