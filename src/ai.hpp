#pragma once

#include <vector>

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "world.hpp"
#include <iostream>

class AISystem
{
public:
	AISystem();

	//~AISystem();

	void step(float elapsed_ms, vec2 window_size_in_game_units);
	void updateEggAiState();
	void EggAiActOnState();

private:
	std::string lastActivePlayer;
	int currentState;
	ECS::Entity horse;
};



