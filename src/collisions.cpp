// Header
#include "collisions.hpp"
#include "render.hpp"
#include "physics.hpp"
#include "tile.hpp"
#include "blobule.hpp"
#include "utils.hpp"
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
	auto blob_blob_collision = [](auto entity, auto entity_other, Direction dir) {

		// entity_other is colliding with entity
		auto& blobMotion1 = ECS::registry<Motion>.get(entity);
		auto& blobMotion2 = ECS::registry<Motion>.get(entity_other);

		// take the vector difference between the centers
		vec2 difference_between_centers = blobMotion1.position - blobMotion2.position;
		// compute the length of this vector
		float distance_between_centers = std::sqrt(dot(difference_between_centers, difference_between_centers));

		// compute the amount you need to move
		float motion1_radius = blobMotion1.scale.x / 2.f;
		float motion2_radius = blobMotion2.scale.x / 2.f;
		float step = motion1_radius + motion2_radius - distance_between_centers;
		vec2 unitDirection = difference_between_centers / distance_between_centers;

		float blobMagnitude = Utils::getVelocityMagnitude(blobMotion1);
		float otherBlobMagnitude = Utils::getVelocityMagnitude(blobMotion2);
		float finalVelocity = (blobMagnitude + otherBlobMagnitude) / 2;

		bool switched = otherBlobMagnitude > blobMagnitude;
		Motion& incomingMotion = !switched ? blobMotion1 : blobMotion2;

		// Calculate angles
		float originalAngle = atan2(incomingMotion.velocity.y, incomingMotion.velocity.x);
		float impactAngle = !switched ?
			atan2(blobMotion2.position.y - blobMotion1.position.y, blobMotion2.position.x - blobMotion1.position.x) :
			atan2(blobMotion1.position.y - blobMotion2.position.y, blobMotion1.position.x - blobMotion2.position.x);

		if (impactAngle < 0 && originalAngle > 0)
			originalAngle = originalAngle - 2*PI;
		else if (impactAngle > 0 && originalAngle < 0)
			originalAngle = originalAngle + 2*PI;

		float derivedAngle = impactAngle > originalAngle ? impactAngle - PI / 2 : impactAngle + PI / 2;

		if (!switched) {
			blobMotion1.velocity = { cos(derivedAngle) * finalVelocity, sin(derivedAngle) * finalVelocity };
			blobMotion2.velocity = { cos(impactAngle) * finalVelocity, sin(impactAngle) * finalVelocity };
		}
		else {
			blobMotion2.velocity = { cos(derivedAngle) * finalVelocity, sin(derivedAngle) * finalVelocity };
			blobMotion1.velocity = { cos(impactAngle) * finalVelocity, sin(impactAngle) * finalVelocity };
		}

		if (blobMotion1.position.x > blobMotion2.position.x)
		{
			blobMotion1.position.x += unitDirection.x * step / 2;
			blobMotion2.position.x -= unitDirection.x * step / 2;
		}
		else
		{
			blobMotion1.position.x += unitDirection.x * step / 2;
			blobMotion2.position.x -= unitDirection.x * step / 2;
		}

		if (blobMotion1.position.y > blobMotion2.position.y)
		{
			blobMotion1.position.y += unitDirection.y * step / 2;
			blobMotion2.position.y -= unitDirection.y * step / 2;
		}
		else
		{
			blobMotion1.position.y += unitDirection.y * step / 2;
			blobMotion2.position.y -= unitDirection.y * step / 2;
		}
	};


	auto blobule_tile_interaction = [](auto entity, auto entity_other, Direction dir) {
		//subject tile to wall
		auto& blob = ECS::registry<Blobule>.get(entity);
		auto& blobMotion = ECS::registry<Motion>.get(entity);
		auto& terrain = ECS::registry<Terrain>.get(entity_other);
		auto& tileMotion = ECS::registry<Motion>.get(entity_other);

		if (terrain.type == Water) {
			blobMotion.velocity = { 0.f, 0.f };
			blobMotion.friction = 0.f;
			blobMotion.position = blob.origin;
		}
		else if (terrain.type == Block)
		{
			float leftEdge = tileMotion.position.x - tileMotion.scale.x / 2;
			float rightEdge = tileMotion.position.x + tileMotion.scale.x / 2;
			float topEdge = tileMotion.position.y - tileMotion.scale.y / 2;
			float bottomEdge = tileMotion.position.y + tileMotion.scale.y / 2;

			float closestX = glm::max(leftEdge, glm::min(blobMotion.position.x, rightEdge));
			float closestY = glm::max(topEdge, glm::min(blobMotion.position.y, bottomEdge));
			std::cout << "closestX: " << closestX << "closestY: " << closestY << std::endl;


			vec2 dist = vec2(blobMotion.position.x - closestX, blobMotion.position.y - closestY);
			std::cout << "distX: " << dist.x << "distY: " << dist.y << std::endl;

			float penetrationDepth = blobMotion.scale.x / 2 - glm::length(dist);
			const vec2 reverseUnitVel = -(blobMotion.velocity / glm::length(blobMotion.velocity));
			while (penetrationDepth > 2)
			{
				std::cout << "pendepth: " << penetrationDepth << std::endl;
				float shiftX = blobMotion.position.x + reverseUnitVel.x * 2;
				float shiftY = blobMotion.position.y + reverseUnitVel.y * 2;
				blobMotion.position = vec2(shiftX, shiftY);
				closestX = glm::max(leftEdge, glm::min(blobMotion.position.x, rightEdge));
				closestY = glm::max(topEdge, glm::min(blobMotion.position.y, bottomEdge));
				dist = vec2(blobMotion.position.x - closestX, blobMotion.position.y - closestY);
				penetrationDepth = blobMotion.scale.x / 2 - glm::length(dist);

			}
			std::cout << "final pos: " << blobMotion.position.x << " " << blobMotion.position.y << std::endl;

			// Top and Bot walls reflect x axis (given by default)
			// Left and Right walls reflect y axis (add 90 to previous angle), reflect, add 90 again
			float blobMagnitude = Utils::getVelocityMagnitude(blobMotion);


			float angle = atan2(blobMotion.velocity.y, blobMotion.velocity.x);

			// Wall collision detected edge
			switch (dir)
			{
			case Direction::Left:
			case Direction::Right:
				angle = -angle - PI;
				break;
			case Direction::Top:
			case Direction::Bottom:
				angle *= -1;
				if (angle == 0) {
					angle += PI;
				}
				break;
			case Direction::Corner:
				angle += PI;
				break;
			default:
				break;
			}

			blobMotion.velocity = { cos(angle) * blobMagnitude, sin(angle) * blobMagnitude };
			//blobMotion.velocity = { 0.f, 0.f};

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
	blobule_blobule_coll.add_observer(blob_blob_collision);
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