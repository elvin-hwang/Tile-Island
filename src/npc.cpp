// Header
#include "npc.hpp"
#include "render.hpp"

ECS::Entity NPC::createNpc(vec2 position)
{
    // Reserve an entity
    auto entity = ECS::Entity();
    
    // Create the rendering components
    std::string key = "npc";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("npc_egg.png"), "textured");
    }
    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);
    
    // Initialize the position, scale and physics components.
    // The only relevant component is position, as the others will not be used.
    auto& motion = ECS::registry<Motion>.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.position = position;
    motion.scale = vec2({1.0f, 1.0f}) * static_cast<vec2>(resource.texture.size);
    
    ECS::registry<NPC>.emplace(entity);
    return entity;
}

