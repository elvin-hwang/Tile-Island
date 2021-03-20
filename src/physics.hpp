#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

enum class Direction {
	unknown = 0,
	Left,
	Right,
	Top,
	Bottom,
	Corner
};

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:
	void step(float elapsed_ms, vec2 window_size_in_game_units);

	static bool is_entity_clicked(ECS::Entity e, float mouse_press_x, float mouse_press_y);

	// Stucture to store collision information
	struct Collision
	{
		// Note, the first object is stored in the ECS container.entities
		ECS::Entity other; // the second object involved in the collision
		Direction direction;
		Collision(ECS::Entity& other);
	};
};
 