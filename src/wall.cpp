// Header
#include "wall.hpp"
#include "render.hpp"

ECS::Entity Wall::createWall(std::string wall_type, vec2 position, float angle)
{
    // Reserve an entity
    auto entity = ECS::Entity();
    
    // Create the rendering components
    std::string key = wall_type;
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(wall_type + ".png"), "textured");
    }
    
    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);
    
    // Initialize the position, scale and physics components.
    // The only relevant component is position, as the others will not be used.
    auto& motion = ECS::registry<Motion>.emplace(entity);
    motion.angle = angle;
    motion.velocity = {0.f, 0.f};
    motion.position = position;
    motion.friction = 0.005f;
    motion.scale = vec2({0.3f, 0.3f}) * static_cast<vec2>(resource.texture.size);
    
    // Create and (empty) Wall component to be able to refer to all walls
    ECS::registry<Wall>.emplace(entity);
    return entity;
}
