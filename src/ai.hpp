#pragma once

#include <vector>

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "world.hpp"
#include <iostream>

// The return type of behaviour tree processing
enum class BTState {
	Running,
	Success,
	Failure
};

// The base class representing any node in our behaviour tree
class BTNode {
public:
	virtual ~BTNode() noexcept = default; // Needed for deletion-through-pointer

	virtual void init(ECS::Entity e) {};

	virtual BTState process(ECS::Entity e) = 0;
};


class AISystem
{
public:
	AISystem();

	//~AISystem();

	void step(float elapsed_ms, vec2 window_size_in_game_units);
	void updateEggAiState();
	void EggAiActOnState();
	ECS::Entity& getActivePlayerBlobule();
	float euclideanDist(Motion motion1, Motion motion2);

private:
	std::string lastActivePlayer;
	int currentState;
	std::shared_ptr <BTNode> root_run_and_return;
	ECS::Entity horse;
};



