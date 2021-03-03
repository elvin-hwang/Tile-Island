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
    motion.velocity = {5.f, 5.f};
    motion.position = position;
    motion.scale = vec2({0.5f, 0.5f}) * static_cast<vec2>(resource.texture.size);
    motion.shape = "circle";
    
    ECS::registry<Egg>.emplace(entity);
    return entity;
}

