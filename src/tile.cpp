// Header
#include "tile.hpp"
#include "render.hpp"

// Initialize size of tiles.
float size = 0.13f;

ECS::Entity Tile::createTile(vec2 position, TerrainType type, std::string blobule_color)
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
            if (blobule_color != "none"){
                key = "tile_blue_" + blobule_color;
            }
            else{
                key = "tile_blue";
            }
        friction = 0.01f;
        motion.isCollidable = false;
        break;
    case Mud:
            if (blobule_color != "none"){
                key = "tile_purple_" + blobule_color;
            }
            else{
                key = "tile_purple";
            }
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
    terrain.position = position;
    terrain.key = key;

    // Create and (empty) Tile component to be able to refer to all tiles
    ECS::registry<Tile>.emplace(entity);
    return entity;
}

void Tile::reloadTile(vec2 position, TerrainType type, std::string blobule_color)
{
    // Look for the tile in the registry.
    for (auto& tile : ECS::registry<Tile>.entities)
    {
        // Calculate approximate location of blobule relative to the tile.
        if (ECS::registry<Terrain>.get(tile).position == position
            && ECS::registry<Terrain>.get(tile).type == type){
            
            // Remove old tile.
            ECS::ContainerInterface::remove_all_components_of(tile);
            
            // Replace old tile with new one with updated blobule_color.
            createTile(position, type, blobule_color);
        }
    }
}

