// Header
#include "blobule.hpp"
#include "render.hpp"

ECS::Entity Blobule::createBlobule(vec2 position, blobuleCol col)
{
    // Reserve an entity
    auto entity = ECS::Entity();
    
    // Create the rendering components
    std::string key = "blobule" + col;
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        std::string path;
        switch (col) {
        case Blue:
            path = textures_path("blobule_blue.png");
            break;
        case Red:
            path = textures_path("blobule_red.png");
            break;
        case Yellow:
            path = textures_path("blobule_yellow.png");
            break;
        case Green:
            path = textures_path("blobule_green.png");
            break;
        default:
            path = textures_path("blobule_blue.png");
        }
        RenderSystem::createSprite(resource, path, "textured");

    }
    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);
    
    // Initialize the position, scale and physics components.
    // The only relevant component is position, as the others will not be used.
    auto& motion = ECS::registry<Motion>.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.position = position;
    motion.friction = 0.f;
    motion.scale = vec2({0.42f, 0.42f}) * static_cast<vec2>(resource.texture.size);
    
    // Create and (empty) Blobule component to be able to refer to all tiles
    ECS::registry<Blobule>.emplace(entity);
    return entity;
}

