// internal
#include "ai.hpp"
#include "tiny_ecs.hpp"
#include "blobule.hpp"

float maxDistanceFromEgg = 10;

void AISystem::step(float elapsed_ms, vec2 window_size_in_game_units)
{
	(void)elapsed_ms; // placeholder to silence unused warning until implemented
	(void)window_size_in_game_units; // placeholder to silence unused warning until implemented

	// egg ai state updated here
	updateState();
	

}

void AISystem::updateState()
{
	// egg ai state updated here
	for (ECS::Entity& eggNPC : ECS::registry<EggAi>.entities)
	{
		Motion eggMotion = eggNPC.get<Motion>();

		for (ECS::Entity& blobule : ECS::registry<Blobule>.entities)
		{
			Motion blobMotion = blobule.get<Motion>();

			float dist = euclideanDist(eggMotion, blobMotion);

			if (dist > maxDistanceFromEgg)
			{
				eggMotion.velocity = vec2(1.f, 1.f);
			}
		}

	}

	// add other states...
}

float euclideanDist(Motion motion1, Motion motion2)
{
	float x = motion1.position.x - motion2.position.x;
	float y = motion1.position.y - motion2.position.y;

	float dist = pow(x, 2) + pow(y, 2);
	return sqrt(dist);
}



