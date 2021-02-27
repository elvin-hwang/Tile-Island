// Header
#include "collisions.hpp"
#include "render.hpp"
#include "physics.hpp"
#include "tile.hpp"
#include "blobule.hpp"
#include "world.hpp"
#include "egg.hpp"

void CollisionSystem::initialize_collisions() {
	
	//observer for blobule - tile collision
	blobule_tile_coll = ECS::registry<Subject>.get(Subject::createSubject("blobule_tile_coll"));
	//observer for blobule - blobule collision
	blobule_blobule_coll = ECS::registry<Subject>.get(Subject::createSubject("blobule_blobule_coll"));

	//Add any collision logic here as a lambda function that takes in (entity, entity_other)
	auto reverse_vel = [](auto entity, auto entity_other) {
		auto& blobMotion = ECS::registry<Motion>.get(entity);
		// auto& tileMotion = ECS::registry<Motion>.get(entity_other);
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
    
    // Lambda function that changes the colour of the tile based on which blobule is on it.
    auto change_tile_colour = [this](auto entity, auto entity_other) {
        auto& player_blobule = ECS::registry<Blobule>.get(entity);
        auto& blobule_motion = ECS::registry<Motion>.get(entity);
        
        std::string color = player_blobule.color;
        auto& current_terrain = ECS::registry<Terrain>.get(entity_other);
        
        if (current_terrain.type == Ice){
            if (color == "red" && current_terrain.key != "tile_blue_red" && blobule_motion.velocity != vec2{0.f, 0.f}){
                
                // Update colour count of tiles.
                if (current_terrain.key == "tile_blue_blue"){
                    // Subtract 1 from blue_tiles
                    blue_tiles--;
                }
                else if (current_terrain.key == "tile_blue_green"){
                    // Subtract 1 from green_tiles
                }
                else if (current_terrain.key == "tile_blue_yellow"){
                    // Subtract 1 from yellow_tiles
                }
                // Add 1 to red_tiles
                
                Tile::reloadTile(current_terrain.position, Ice, "red");
                for (auto& egg : ECS::registry<Egg>.entities) {
                    Egg::reloadEgg(ECS::registry<Motion>.get(egg).position);
                }
            }
            else if (color == "green" && current_terrain.key != "tile_blue_green" && blobule_motion.velocity != vec2{0.f, 0.f}){
                
                // Update colour count of tiles.
                if (current_terrain.key == "tile_blue_blue"){
                    // Subtract 1 from blue_tiles
                    blue_tiles--;
                }
                else if (current_terrain.key == "tile_blue_red"){
                    // Subtract 1 from red_tiles
                    red_tiles--;
                }
                else if (current_terrain.key == "tile_blue_yellow"){
                    // Subtract 1 from yellow_tiles
                    yellow_tiles--;
                }
                // Add 1 to green_tiles
                green_tiles++;
                
                Tile::reloadTile(current_terrain.position, Ice, "green");
                for (auto& egg : ECS::registry<Egg>.entities) {
                    Egg::reloadEgg(ECS::registry<Motion>.get(egg).position);
                }
            }
            else if (color == "yellow" && current_terrain.key != "tile_blue_yellow" && blobule_motion.velocity != vec2{0.f, 0.f}){
                
                // Update colour count of tiles.
                if (current_terrain.key == "tile_blue_blue"){
                    // Subtract 1 from blue_tiles
                    blue_tiles--;
                }
                else if (current_terrain.key == "tile_blue_green"){
                    // Subtract 1 from green_tiles
                    green_tiles--;
                }
                else if (current_terrain.key == "tile_blue_red"){
                    // Subtract 1 from red_tiles
                    red_tiles--;
                }
                // Add 1 to yellow_tiles
                yellow_tiles++;
                
                Tile::reloadTile(current_terrain.position, Ice, "yellow");
                for (auto& egg : ECS::registry<Egg>.entities) {
                    Egg::reloadEgg(ECS::registry<Motion>.get(egg).position);
                }
            }
            else if (color == "blue" && current_terrain.key != "tile_blue_blue" && blobule_motion.velocity != vec2{0.f, 0.f}){
                
                // Update colour count of tiles.
                if (current_terrain.key == "tile_blue_red"){
                    // Subtract 1 from red_tiles
                    red_tiles--;
                }
                else if (current_terrain.key == "tile_blue_green"){
                    // Subtract 1 from green_tiles
                    green_tiles--;
                }
                else if (current_terrain.key == "tile_blue_yellow"){
                    // Subtract 1 from yellow_tiles
                    yellow_tiles--;
                }
                // Add 1 to blue_tiles
                blue_tiles++;
                
                Tile::reloadTile(current_terrain.position, Ice, "blue");
                for (auto& egg : ECS::registry<Egg>.entities) {
                    Egg::reloadEgg(ECS::registry<Motion>.get(egg).position);
                }
            }
        }
        else if (current_terrain.type == Mud){
            if (color == "red" && current_terrain.key != "tile_purple_red" && blobule_motion.velocity != vec2{0.f, 0.f}){
                
                // Update colour count of tiles.
                if (current_terrain.key == "tile_purple_blue"){
                    // Subtract 1 from blue_tiles
                    blue_tiles--;
                }
                else if (current_terrain.key == "tile_purple_green"){
                    // Subtract 1 from green_tiles
                    green_tiles--;
                }
                else if (current_terrain.key == "tile_purple_yellow"){
                    // Subtract 1 from yellow_tiles
                    yellow_tiles--;
                }
                // Add 1 to red_tiles
                red_tiles++;
                
                Tile::reloadTile(current_terrain.position, Mud, "red");
                for (auto& egg : ECS::registry<Egg>.entities) {
                    Egg::reloadEgg(ECS::registry<Motion>.get(egg).position);
                }
            }
            else if (color == "green" && current_terrain.key != "tile_purple_green" && blobule_motion.velocity != vec2{0.f, 0.f}){
                
                // Update colour count of tiles.
                if (current_terrain.key == "tile_purple_blue"){
                    // Subtract 1 from blue_tiles
                    blue_tiles--;
                }
                else if (current_terrain.key == "tile_purple_red"){
                    // Subtract 1 from red_tiles
                    red_tiles--;
                }
                else if (current_terrain.key == "tile_purple_yellow"){
                    // Subtract 1 from yellow_tiles
                    yellow_tiles--;
                }
                // Add 1 to green_tiles
                green_tiles++;
                
                Tile::reloadTile(current_terrain.position, Mud, "green");
                for (auto& egg : ECS::registry<Egg>.entities) {
                    Egg::reloadEgg(ECS::registry<Motion>.get(egg).position);
                }
            }
            else if (color == "yellow" && current_terrain.key != "tile_purple_yellow" && blobule_motion.velocity != vec2{0.f, 0.f}){
                
                // Update colour count of tiles.
                if (current_terrain.key == "tile_purple_blue"){
                    // Subtract 1 from blue_tiles
                    blue_tiles--;
                }
                else if (current_terrain.key == "tile_purple_green"){
                    // Subtract 1 from green_tiles
                    green_tiles--;
                }
                else if (current_terrain.key == "tile_purple_red"){
                    // Subtract 1 from red_tiles
                    red_tiles--;
                }
                // Add 1 to yellow_tiles
                yellow_tiles++;
                
                Tile::reloadTile(current_terrain.position, Mud, "yellow");
                for (auto& egg : ECS::registry<Egg>.entities) {
                    Egg::reloadEgg(ECS::registry<Motion>.get(egg).position);
                }
            }
            else if (color == "blue" && current_terrain.key != "tile_purple_blue" && blobule_motion.velocity != vec2{0.f, 0.f}){
                
                // Update colour count of tiles.
                if (current_terrain.key == "tile_purple_red"){
                    // Subtract 1 from red_tiles
                    red_tiles--;
                }
                else if (current_terrain.key == "tile_purple_green"){
                    // Subtract 1 from green_tiles
                    green_tiles--;
                }
                else if (current_terrain.key == "tile_purple_yellow"){
                    // Subtract 1 from yellow_tiles
                    yellow_tiles--;
                }
                // Add 1 to blue_tiles
                blue_tiles++;
                
                Tile::reloadTile(current_terrain.position, Mud, "blue");
                for (auto& egg : ECS::registry<Egg>.entities) {
                    Egg::reloadEgg(ECS::registry<Motion>.get(egg).position);
                }
            }
        }
    };
    
	//add lambdas to the observer lists
	blobule_tile_coll.add_observer(change_blobule_friction);
    blobule_tile_coll.add_observer(change_tile_colour);
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
				blobule_tile_coll.notify(entity,entity_other);
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
