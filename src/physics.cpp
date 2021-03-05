// internal
#include "physics.hpp"
#include "tiny_ecs.hpp"
#include "debug.hpp"
#include "blobule.hpp"
#include <iostream>
#include <egg.hpp>

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// fabs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

/*
Our Guide on box circle collisions
https://www.figma.com/file/K32AjU7kJXHVw9BpALdTY1/Untitled?node-id=0%3A1
Entities are positioned at the CENTER of their texture, which means that if you place an entity at { 0.f, 0.f }, it would only show the
bottom right corner of that texture. Use this knowledge to determine where centers and edges of entities are!
*/
Direction box_circle_collides(const Motion& box, const Motion& circle)
{
	// Define edges of box
	float top_edge = box.position.y - box.scale.y / 2.f; // y1
	float right_edge = box.position.x + box.scale.x / 2.f; // x2
	float bottom_edge = box.position.y + box.scale.y / 2.f; // y2
	float left_edge = box.position.x - box.scale.x / 2.f; // x1

	// Define circle radius and center of circle position
	float circle_radius = circle.scale.x / 2.f;
	vec2 center_of_circle = circle.position;

	// Right edge collision
	if (center_of_circle.x > right_edge
		&& abs(right_edge - center_of_circle.x) <= circle_radius
		&& top_edge - circle_radius <= center_of_circle.y
		&& center_of_circle.y <= bottom_edge + circle_radius)
		return Direction::Right;
	// Top edge collision
	else if (center_of_circle.y < top_edge
		&& abs(top_edge - center_of_circle.y) <= circle_radius
		&& left_edge - circle_radius <= center_of_circle.x
		&& center_of_circle.x <= right_edge + circle_radius)
		return Direction::Top;
	// Bottom edge collision
	else if (center_of_circle.y > bottom_edge
		&& abs(bottom_edge - center_of_circle.y) <= circle_radius
		&& left_edge - circle_radius <= center_of_circle.x
		&& center_of_circle.x <= right_edge + circle_radius)
		return Direction::Bottom;
	// Left edge collision
	else if (center_of_circle.x < left_edge
		&& abs(left_edge - center_of_circle.x) <= circle_radius
		&& top_edge - circle_radius <= center_of_circle.y
		&& center_of_circle.y <= bottom_edge + circle_radius)
		return Direction::Left;
	else
		return Direction::unknown;
}

// circle circle collision check
bool circle_circle_collides(const Motion& motion1, const Motion& motion2)
{
	vec2 motion1_center = motion1.position;
	vec2 motion2_center = motion2.position;
	vec2 difference_between_centers = motion1_center - motion2_center;
	float distance_between_centers = std::sqrt(dot(difference_between_centers, difference_between_centers));
	float motion1_radius = motion1.scale.x / 2.f;
	float motion2_radius = motion2.scale.x / 2.f;
	return distance_between_centers < motion1_radius + motion2_radius;
}

//bool box_box_collides(const Motion& motion1, const Motion& motion2)
//{
//
//}

void PhysicsSystem::step(float elapsed_ms, vec2 window_size_in_game_units)
{
	// Move entities based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.

	for (auto& motion : ECS::registry<Motion>.components)
	{
		float step_seconds = 1.0f * (elapsed_ms / 1000.f);
		motion.position = motion.position + (step_seconds * motion.velocity);
	}

	(void)elapsed_ms; // placeholder to silence unused warning until implemented
	(void)window_size_in_game_units;

	// Visualization for debugging the position and scale of objects
	if (DebugSystem::in_debug_mode)
	{
		for (auto& motion : ECS::registry<Motion>.components)
		{
			DebugSystem::createBox(motion.position, motion.scale);
		}
	}

	// Go through the list of Blobules rather Motion
	// For each blobule check what its colliding with using the collision detection functions above for each Motion
	// We need nested for loop, inner loop goes through all of the motion, double check that the inner loop doesn't go through the blobule it's currently on (entity.id)
	// 


	// Check for collisions between all moving entities
	auto& blobule_container = ECS::registry<Blobule>;
	auto& motion_container = ECS::registry<Motion>;
	// for (auto [i, motion_i] : enumerate(motion_container.components)) // in c++ 17 we will be able to do this instead of the next three lines
	for (unsigned int i=0; i<blobule_container.components.size(); i++)
	{
		ECS::Entity blob_entity_i = blobule_container.entities[i];
		Motion& blob_motion_i = ECS::registry<Motion>.get(blob_entity_i);

		for (unsigned int j = 0; j < motion_container.components.size(); j++)
		{
			Motion& motion_j = motion_container.components[j];
			ECS::Entity entity_j = motion_container.entities[j];

			if (entity_j.id == blob_entity_i.id) {
				continue;
			}

			// motion_j can be square, circle (later might be egg shaped)

			// temporarily egg is considered a circle and follows circle/circle and circle/square collisions, 
			// in m3 we need to implement precise collision with the egg mesh and handle the collision check differently
			if (motion_j.shape == "square")
			{
				// Blobule vs Tile
				Direction collisionEdge = box_circle_collides(blob_motion_i, motion_j);
				if (collisionEdge != Direction::unknown)
				{
					auto& collision = ECS::registry<Collision>.emplace_with_duplicates(blob_entity_i, entity_j);
					collision.direction = collisionEdge;
				}
			}
			else if (motion_j.shape == "circle")
			{
				// Blobule vs Blobule
				if (circle_circle_collides(blob_motion_i, motion_j))
				{
					ECS::registry<Collision>.emplace_with_duplicates(blob_entity_i, entity_j);
				}
			}
		}
	}
}

PhysicsSystem::Collision::Collision(ECS::Entity& other)
{
	this->other = other;
}
