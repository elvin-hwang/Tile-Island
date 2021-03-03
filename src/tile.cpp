// Header
#include "tile.hpp"
#include "render.hpp"
#include "blobule.hpp"

// Initialize size of tiles.
float size = 0.13f;

ECS::Entity Tile::createTile(vec2 position, TerrainType type)
{
    // Reserve an entity
    auto entity = ECS::Entity();

    // Type specific variables
    std::string key = ""; // Key is the texture file name without the ".png"
    float friction = 0.f;

    auto& motion = ECS::registry<Motion>.emplace(entity);
    switch (type) {
    case Water:
        key = "tile_water";
        motion.isCollidable = true;
        break;
    case Block:
        key = "tile_grey";
        motion.isCollidable = true;
        break;
    case Ice:
        key = "tile_blue";
        friction = 0.01f;
        motion.isCollidable = false;
        break;
    case Mud:
        key = "tile_purple";
        friction = 0.04f;
        motion.isCollidable = false;
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

    // Reserve an entity
    auto splatEntity = ECS::Entity();
    ShadedMesh& resource2 = cache_resource("splat_yellow");

    if (resource2.effect.program.resource == 0)
    {
        resource2 = ShadedMesh();
        RenderSystem::createSprite(resource2, textures_path("splat_yellow.png"), "textured");

    }
    ECS::registry<ShadedMeshRef>.emplace(splatEntity, resource2);

    // Initialize the position, scale and physics components.
    // The only relevant component is position, as the others will not be used.
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.position = position;
    motion.scale = vec2({ size, size }) * static_cast<vec2>(resource.texture.size);
    motion.shape = "square";
    
    auto& terrain = ECS::registry<Terrain>.emplace(entity);
    terrain.type = type;
    terrain.friction = friction;

    // Create and (empty) Tile component to be able to refer to all tiles
    auto& tile = ECS::registry<Tile>.emplace(entity);
    tile.splatEntity = ECS::Entity();
    return entity;
}

void Tile::setSplat(ECS::Entity entity, blobuleCol color) {
    if (!ECS::registry<Tile>.has(entity)) {
        return;
    }
    auto& terrain = ECS::registry<Terrain>.get(entity);
    if (terrain.type == Water || terrain.type == Block) {
        return;
    }

    auto& tile = ECS::registry<Tile>.get(entity);
    ECS::Entity splatEntity = tile.splatEntity;
    ECS::ContainerInterface::remove_all_components_of(splatEntity);

    std::string key = "splat_";

    switch (color) {
    case blobuleCol::Blue:
        key += "blue";
        ECS::registry<BlueSplat>.emplace(splatEntity);
        break;
    case blobuleCol::Green:
        key += "green";
        ECS::registry<GreenSplat>.emplace(splatEntity);
        break;
    case blobuleCol::Red:
        key += "red";
        ECS::registry<RedSplat>.emplace(splatEntity);
        break;
    case blobuleCol::Yellow:
        key += "yellow";
        ECS::registry<YellowSplat>.emplace(splatEntity);
        break;
    default:
        break;
    }

    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path(key.append(".png")), "textured");

    }
    ECS::registry<ShadedMeshRef>.emplace(splatEntity, resource);

    auto& tileMotion = ECS::registry<Motion>.get(entity);
    auto& splatMotion = ECS::registry<Motion>.emplace(splatEntity);
    splatMotion.position = tileMotion.position;
    splatMotion.scale = vec2({ size * 0.6, size * 0.6 }) * static_cast<vec2>(resource.texture.size);
    splatMotion.isCollidable = false;
}

