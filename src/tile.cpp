// Header
#include "tile.hpp"
#include "render.hpp"

// Initialize size of tiles.
float size = 0.13f;

ECS::Entity Tile::createTile(vec2 position, TerrainType type)
{
    // Reserve an entity
    auto entity = ECS::Entity();

    // Type specific variables
    std::string key = ""; // Key is the texture file name without the ".png"
    float friction = 0.f;

    switch (type) {
    case Water:
        key = "tile_water";
        break;
    case Block:
        key = "tile_grey";
        break;
    case Ice:
        key = "tile_blue";
        friction = 0.01f;
        break;
    case Mud:
        key = "tile_purple";
        friction = 0.04f;
        break;
    default:
        break;
    }

    // Create the rendering components
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(key.append(".png")), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);

    // Initialize the position, scale and physics components.
    // The only relevant component is position, as the others will not be used.
    auto& motion = ECS::registry<Motion>.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.position = position;
    motion.scale = vec2({ size, size }) * static_cast<vec2>(resource.texture.size);

    auto& terrain = ECS::registry<Terrain>.emplace(entity);
    terrain.type = type;
    terrain.friction = friction;

    // Create and (empty) Tile component to be able to refer to all tiles
    ECS::registry<Tile>.emplace(entity);
    return entity;
}
