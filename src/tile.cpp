// Header
#include "tile.hpp"
#include "render.hpp"

ECS::Entity Tile::createWaterTile(vec2 position)
{
    // Reserve an entity
    auto entity = ECS::Entity();

    // Create the rendering components
    std::string key = "tile_water";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("tile_water.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);

    // Initialize the position, scale and physics components.
    // The only relevant component is position, as the others will not be used.
    auto& motion = ECS::registry<Motion>.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.position = position;
    motion.scale = vec2({ 0.3f, 0.3f }) * static_cast<vec2>(resource.texture.size);

    auto& terrain = ECS::registry<Terrain>.emplace(entity);
    terrain.type = Water;
    terrain.friction = 0.0f;


    // Create and (empty) Tile component to be able to refer to all tiles
    ECS::registry<Tile>.emplace(entity);
    return entity;
}

ECS::Entity Tile::createBlueTile(vec2 position)
{
    // Reserve an entity
    auto entity = ECS::Entity();
    
    // Create the rendering components
    std::string key = "tile_blue";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("tile_blue.png"), "textured");
    }
    
    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);
    
    // Initialize the position, scale and physics components.
    // The only relevant component is position, as the others will not be used.
    auto& motion = ECS::registry<Motion>.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.position = position;
    motion.scale = vec2({0.3f, 0.3f}) * static_cast<vec2>(resource.texture.size);

    auto& terrain = ECS::registry<Terrain>.emplace(entity);
    terrain.type = Ice;
    terrain.friction = 0.005f;
    
    // Create and (empty) Tile component to be able to refer to all tiles
    ECS::registry<Tile>.emplace(entity);
    return entity;
}

ECS::Entity Tile::createPurpleTile(vec2 position)
{
    // Reserve an entity
    auto entity = ECS::Entity();
    
    // Create the rendering components
    std::string key = "tile_purple";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("tile_purple.png"), "textured");
    }
    
    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);
    
    // Initialize the position, scale and physics components.
    // The only relevant component is position, as the others will not be used.
    auto& motion = ECS::registry<Motion>.emplace(entity);
    motion.angle = 0.f;
    motion.velocity = {0.f, 0.f};
    motion.position = position;
    motion.scale = vec2({0.3f, 0.3f}) * static_cast<vec2>(resource.texture.size);

    auto& terrain = ECS::registry<Terrain>.emplace(entity);
    terrain.type = Ice;
    terrain.friction = 0.02f;
    
    // Create and (empty) Tile component to be able to refer to all tiles
    ECS::registry<Tile>.emplace(entity);
    return entity;
}
