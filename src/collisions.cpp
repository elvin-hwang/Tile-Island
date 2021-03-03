// Header
#include "collisions.hpp"
#include "render.hpp"
#include "physics.hpp"
#include "tile.hpp"
#include "blobule.hpp"

void CollisionSystem::initialize_collisions() {

	//observer for blobule - tile collision
	blobule_tile_coll = ECS::registry<Subject>.get(Subject::createSubject("blobule_tile_coll"));
	//observer for blobule - blobule collision
	blobule_blobule_coll = ECS::registry<Subject>.get(Subject::createSubject("blobule_blobule_coll"));

	//Add any collision logic here as a lambda function that takes in (entity, entity_other)
	auto reverse_vel = [](auto entity, auto entity_other) {
		auto& blobMotion = ECS::registry<Motion>.get(entity);
		auto& tileMotion = ECS::registry<Motion>.get(entity_other);
		blobMotion.velocity = -blobMotion.velocity;
	};

	auto change_blobule_friction = [](auto entity, auto entity_other) {
		//subject tile to wall
		auto& blob = ECS::registry<Blobule>.get(entity);
		auto& blobMotion = ECS::registry<Motion>.get(entity);
		auto& terrain = ECS::registry<Terrain>.get(entity_other);

		if (terrain.type == Water) {
			blobMotion.velocity = { 0.f, 0.f };
			blobMotion.friction = 0.f;
			blobMotion.position = blob.origin;
		}
		else if (terrain.type == Block)
		{
			blobMotion.velocity = { 0.f, 0.f };
		}
		else {
			blobMotion.friction = terrain.friction;
		}
	};

	auto change_tile_color = [](auto entity, auto entity_other) {
		auto tileMotion = ECS::registry<Motion>.get(entity_other);
		auto blobMotion = ECS::registry<Motion>.get(entity);
		vec2 difference_between_centers = tileMotion.position - blobMotion.position;
		float distance_between_centers = std::sqrt(dot(difference_between_centers, difference_between_centers));
		float blobMotion_hit_radius = blobMotion.scale.x / 1.3f;
		if (distance_between_centers <= blobMotion_hit_radius) {
			auto& blob = ECS::registry<Blobule>.get(entity);
			Tile::setSplat(entity_other, blob.colEnum);
		}
	};

	//add lambdas to the observer lists
	blobule_tile_coll.add_observer(change_blobule_friction);
	blobule_tile_coll.add_observer(change_tile_color);
	blobule_blobule_coll.add_observer(reverse_vel);

}
// Compute collisions between entities
void CollisionSystem::handle_collisions()
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
				blobule_tile_coll.notify(entity, entity_other);
			}

			// Blobule - blobule collisions
			if (ECS::registry<Blobule>.has(entity_other)) {
				blobule_blobule_coll.notify(entity, entity_other);
			}
		}
	}
	// Remove all collisions from this simulation step
	ECS::registry<PhysicsSystem::Collision>.clear();
}