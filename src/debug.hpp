#pragma once

#include "common.hpp"

// Data structure for pebble-specific information
namespace DebugSystem {
	extern bool in_debug_mode;

	// draw a red line for debugging purposes
	void createLine(vec2 position, vec2 size, float angle);

	// debug in bounding boxes
	void createBox(vec2 position, vec2 size, float angle);

	void createDirectionLine(vec2 position, vec2 velocity, vec2 size);

	// Removes all debugging graphics in ECS, called at every iteration of the game loop
	void clearDebugComponents();
};
