#pragma once

#include <vector>

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "world.hpp"

class AISystem
{
public:
	void step(float elapsed_ms, vec2 window_size_in_game_units);
	void updateState();
	float euclideanDist(Motion motion1, Motion motion2);
};
