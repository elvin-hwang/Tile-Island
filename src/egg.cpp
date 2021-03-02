// Header
#include "egg.hpp"
#include "render.hpp"

ECS::Entity Egg::createEgg(vec2 position)
{
    // Reserve an entity
    auto entity = ECS::Entity();
    
    // Create the rendering components
    std::string key = "egg";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("npc_egg.png"), "textured");
    }
    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);
    // adding reference to eggAi
    ECS::registry<EggAi>.emplace(entity);
    
    // Initialize the position, scale and physics components.
    // The only relevant component is position, as the others will not be used.
    auto& motion = ECS::registry<Motion>.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.position = position;
    motion.scale = vec2({0.5f, 0.5f}) * static_cast<vec2>(resource.texture.size);
    
    ECS::registry<Egg>.emplace(entity);
    return entity;
}

void Egg::reloadEgg(vec2 position)
{
    // Look for the tile in the registry.
    for (auto& egg : ECS::registry<Egg>.entities)
    {
        // Remove old egg.
        ECS::ContainerInterface::remove_all_components_of(egg);
            
        // Replace old egg with new one with old egg's position.
        createEgg(position);
    }
}

