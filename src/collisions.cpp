// Header
#include "collisions.hpp"
#include "render.hpp"
#include "physics.hpp"
#include "tile.hpp"
#include "blobule.hpp"
#include "wall.hpp"

void Collisions::initialize_collisions() {

	auto reverse_vel = [](auto& motion) {
		motion.velocity = -motion.velocity;
	}

	blobule_tile_coll.add(reverse_val)
}
// Compute collisions between entities
void Collisions::handle_collisions()
{
	// Loop over all collisions detected by the physics system
	auto& registry = ECS::registry<PhysicsSystem::Collision>;
	for (unsigned int i = 0; i < registry.components.size(); i++)
	{
		// The entity and its collider
		auto entity = registry.entities[i];
		auto entity_other = registry.components[i].other;

		// Blobule collisions
		if (ECS::registry<Blobule>.has(entity)) {
			// Change friction of blobule based on which tile it is on
			if (ECS::registry<Tile>.has(entity_other)) {

				//subject tile to wall
				auto& blob = ECS::registry<Blobule>.get(entity);
				auto& blobMotion = ECS::registry<Motion>.get(entity);
				auto& terrain = ECS::registry<Terrain>.get(entity_other);

				if (terrain.type == Water) {
					blobMotion.velocity = { 0.f, 0.f };
					blobMotion.friction = 0.f;
					blobMotion.position = blob.origin;
				}
				else {
					blobMotion.friction = terrain.friction;
				}
				blobule_tile_coll.notify();
			}

			// Blobule - wall collisions
			if (ECS::registry<Wall>.has(entity_other)) {
				blobule_wall_coll.notify();
				//subject - blob to wall
				auto& blobMotion = ECS::registry<Motion>.get(entity);
				auto& tileMotion = ECS::registry<Motion>.get(entity_other);
				blobMotion.velocity = -blobMotion.velocity;
			}
		}
	}

	// Remove all collisions from this simulation step
	ECS::registry<PhysicsSystem::Collision>.clear();
}
