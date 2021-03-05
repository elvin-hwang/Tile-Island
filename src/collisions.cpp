// Header
#include "collisions.hpp"
#include "render.hpp"
#include "physics.hpp"
#include "tile.hpp"
#include "blobule.hpp"
#include <egg.hpp>
#include <iostream>

void CollisionSystem::initialize_collisions() {

	//observer for blobule - tile collision
	blobule_tile_coll = ECS::registry<Subject>.get(Subject::createSubject("blobule_tile_coll"));
	// observer for blobule - blobule collision (circle-circle)
	blobule_blobule_coll = ECS::registry<Subject>.get(Subject::createSubject("blobule_blobule_coll"));
	// observer for blobule - egg collision (circle-preciseCollision)
	blobule_egg_coll = ECS::registry<Subject>.get(Subject::createSubject("blobule_egg_coll"));

	egg_tile_coll = ECS::registry<Subject>.get(Subject::createSubject("egg_tile_coll"));

	//Add any collision logic here as a lambda function that takes in (entity, entity_other)
	auto reverse_vel = [](auto entity, auto entity_other, Direction dir) {
		auto& blobMotion = ECS::registry<Motion>.get(entity);
		auto& tileMotion = ECS::registry<Motion>.get(entity_other);
		blobMotion.velocity = -blobMotion.velocity;
	};

	auto blobule_tile_interaction = [](auto entity, auto entity_other, Direction dir) {
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
			// TODO: Implement more advanced particle collision rebounding effect
			//blobMotion.velocity = { 0.f, 0.f };
			blobMotion.velocity = -blobMotion.velocity;

		}
		else {
			blobMotion.friction = terrain.friction;
		}
	};

	auto change_tile_color = [](auto entity, auto entity_other, Direction dir) {
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

	auto egg_tile_interaction = [](auto entity, auto entity_other, Direction dir) {

		auto& eggMotion = ECS::registry<Motion>.get(entity);
		auto& terrain = ECS::registry<Terrain>.get(entity_other);

		if (terrain.type == Water || terrain.type == Block)
		{
			std::cout << "COLLISION" << std::endl;
			eggMotion.velocity = -eggMotion.velocity;
			eggMotion.direction = -eggMotion.direction;
		}
	};

	// egg disappears on collision with blob (only use the second param)
	auto remove_egg = [](auto entity, auto eggEntity, Direction dir) {
		ECS::ContainerInterface::remove_all_components_of(eggEntity);
	};

	//add lambdas to the observer lists
	blobule_tile_coll.add_observer(blobule_tile_interaction);
	blobule_tile_coll.add_observer(change_tile_color);
	blobule_blobule_coll.add_observer(reverse_vel);
	blobule_egg_coll.add_observer(remove_egg);
	egg_tile_coll.add_observer(egg_tile_interaction);
}
// Compute collisions between entities
void CollisionSystem::handle_collisions()
{
	// Loop over all collisions detected by the physics system
	auto& registry = ECS::registry<PhysicsSystem::Collision>;
	for (unsigned int i = 0; i < registry.components.size(); i++)
	{
		auto& collision = registry.components[i];
		// The entity and its collider
		auto entity = registry.entities[i];
		auto entity_other = collision.other;

		// Blobule collisions
		if (ECS::registry<Blobule>.has(entity)) {
			// Change friction of blobule based on which tile it is on
			if (ECS::registry<Tile>.has(entity_other)) {
				blobule_tile_coll.notify(entity, entity_other, collision.direction);
			}

			// Blobule - blobule collisions
			if (ECS::registry<Blobule>.has(entity_other)) {
				blobule_blobule_coll.notify(entity, entity_other, collision.direction);
			}

			// blobule - egg collisions
			if (ECS::registry<Egg>.has(entity_other)) {
				blobule_egg_coll.notify(entity, entity_other, collision.direction);
			}
		}

		// Egg - collisions
		else if (ECS::registry<Egg>.has(entity)) {
			if (ECS::registry<Tile>.has(entity_other)) {
				egg_tile_coll.notify(entity, entity_other, collision.direction);
			}
		}
	}
	// Remove all collisions from this simulation step
	ECS::registry<PhysicsSystem::Collision>.clear();
}