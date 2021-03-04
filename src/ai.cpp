// internal
#include "ai.hpp"
#include "tiny_ecs.hpp"
#include "blobule.hpp"
#include "utils.hpp"
#include "behaviourTree.hpp"

#include <iostream>

float maxDistanceFromEgg = 100.f;
float eggSpeed = 75.f;

// defined these as lambdas as thats how the tutorial example had them
auto euclideanDist = [](Motion& fishMotion, Motion& turtleMotion)
{
	float x = fishMotion.position.x - turtleMotion.position.x;
	float y = fishMotion.position.y - turtleMotion.position.y;

	float dist = (float)pow(x, 2) + (float)pow(y, 2);
	return sqrt(dist);
};

auto checkNearbyBlobules = [](ECS::Entity e) {
	for (ECS::Entity& blob : ECS::registry<Blobule>.entities)
	{
		Motion& eggMotion = ECS::registry<Motion>.get(e);
		Motion& blobMotion = ECS::registry<Motion>.get(blob);
		if (euclideanDist(eggMotion, blobMotion) <= maxDistanceFromEgg)
			return true;
	}
	return false;
};

// defining all the leaf nodes
std::shared_ptr <BTNode> moveX = std::make_unique<MoveXDirection>(20, eggSpeed);
std::shared_ptr <BTNode> moveY = std::make_unique<MoveYDirection>(20, eggSpeed);
std::shared_ptr <BTNode> turnX = std::make_unique<TurnX>();
std::shared_ptr <BTNode> turnY = std::make_unique<TurnY>();
std::shared_ptr <BTNode> flee = std::make_unique<Flee>();

// defining composite nodes 
std::shared_ptr <BTNode> SquareMovementPattern = std::make_unique<BTAndSequence>(std::vector<std::shared_ptr <BTNode>>({ moveX, turnX, moveY, turnY }));
std::shared_ptr <BTNode> moveOrFlee = std::make_unique<BTIfElseCondition>(flee, SquareMovementPattern, checkNearbyBlobules);
std::shared_ptr <BTNode> eggBehaviour = std::make_unique<BTRepeatingSequence>(std::vector<std::shared_ptr <BTNode>>({ moveOrFlee }));

void AISystem::step(float elapsed_ms, vec2 window_size_in_game_units)
{
	(void)elapsed_ms; // placeholder to silence unused warning until implemented
	(void)window_size_in_game_units; // placeholder to silence unused warning until implemented

	for (ECS::Entity& eggNPC : ECS::registry<EggAi>.entities)
	{
		EggAi& eggAi = ECS::registry<EggAi>.get(eggNPC);
		if (!eggAi.initBehaviour)
		{
			eggBehaviour->init(eggNPC);
			eggAi.initBehaviour = true;
		}
		eggBehaviour->process(eggNPC);
	}

	// add other ai steps...
}
