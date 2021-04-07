// Header
#include <camera.hpp>

ECS::Entity Camera::createCamera(vec2 position) 
{
    // Reserve an entity
    auto entity = ECS::Entity();

    // Initialize the position, scale and physics components.
    // The only relevant component is position, as the others will not be used.
    auto& motion = ECS::registry<Motion>.emplace(entity);
    motion.position = position;
    motion.scale = vec2(1.f, 1.f);

    ECS::registry<Camera>.emplace(entity);
    return entity;
}