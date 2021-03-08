// Header
#include "blobule.hpp"
#include "render.hpp"

ECS::Entity Blobule::createBlobule(vec2 position, blobuleCol col, std::string colString)
{
    // Reserve an entity
    auto entity = ECS::Entity();

    // Create the rendering components
    std::string key = "blobule" + colString;
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        resource.num_rows = 2.f;
        resource.num_columns = 3.f;
        std::string path;
        switch (col) {
        case blobuleCol::Blue:
            path = textures_path("blue.png");
            break;
        case blobuleCol::Red:
            path = textures_path("red.png");
            break;
        case blobuleCol::Yellow:
            path = textures_path("yellow.png");
            break;
        case blobuleCol::Green:
            path = textures_path("green.png");
            break;
        default:
            path = textures_path("blue.png");
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
    motion.scale = vec2({ 0.80f, 0.80f }) * vec2({ resource.texture.size.x / resource.num_columns, resource.texture.size.y / resource.num_rows });
    motion.isCollidable = true;
    motion.shape = "circle";

    // Create and (empty) Blobule component to be able to refer to all tiles
    auto& blob = ECS::registry<Blobule>.emplace(entity);
    blob.origin = position;
    blob.color = colString;

    return entity;
}

