// Header
#include "tile.hpp"
#include "render.hpp"
#include "blobule.hpp"
#include <iostream>
#include "common.hpp"

// Initialize size of tiles.
float size = 0.13f;

std::unordered_map<int, std::string> colorMap;

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
        friction = 0.03f;
        motion.isCollidable = false;
        break;
    case Sand:
        key = "tile_brown";
        friction = 0.02f;
        motion.isCollidable = false;
        break;
    case Acid:
        key = "tile_green";
        friction = 0.08f;
        motion.isCollidable = false;
        break;
    case Speed:
        key = "tile_speed";
        friction = 0.01f;
        motion.isCollidable = false;
        break;
    case Speed_UP:
        key = "tile_speed_up";
        friction = 0.01f;
        motion.isCollidable = false;
        break;
    case Speed_LEFT:
        key = "tile_speed_left";
        friction = 0.01f;
        motion.isCollidable = false;
        break;
    case Speed_RIGHT:
        key = "tile_speed_right";
        friction = 0.01f;
        motion.isCollidable = false;
        break;
    case Speed_DOWN:
        key = "tile_speed_down";
        friction = 0.01f;
        motion.isCollidable = false;
        break;
    case Teleport:
        key = "tile_teleport";
        friction = 0.01f;
        motion.isCollidable = false;
        ECS::registry<Teleporting>.emplace(entity);
        break;
    default:
        break;
    }

    // Create the rendering components
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        if (key == "tile_water")
        {
            std::vector<ColoredVertex> vertices;
            std::vector<uint16_t> indices;

            constexpr float z = -0.1f;

            ColoredVertex v;
            v.position = { -0.5f, -0.5f, z };
            v.color = { 0.8, 0.8, 0.8 };
            vertices.push_back(v);

            v.position = { -0.5f, 0.5f, z };
            v.color = { 0.8, 0.8, 0.8 };
            vertices.push_back(v);

            v.position = { 0.5f, 0.5f, z };
            v.color = { 0.8, 0.8, 0.8 };
            vertices.push_back(v);

            v.position = { 0.5f, -0.5f, z };
            v.color = { 0.8, 0.8, 0.8 };
            vertices.push_back(v);

            indices.push_back(static_cast<uint16_t>(0));
            indices.push_back(static_cast<uint16_t>(1));
            indices.push_back(static_cast<uint16_t>(2));
            indices.push_back(static_cast<uint16_t>(2));
            indices.push_back(static_cast<uint16_t>(3));
            indices.push_back(static_cast<uint16_t>(0));

            resource.mesh.vertices = vertices;
            resource.mesh.vertex_indices = indices;
            RenderSystem::createColoredMesh(resource, "colored_mesh");
        }

        else {
            resource = ShadedMesh();
            RenderSystem::createSprite(resource, textures_path(key.append(".png")), "textured");
        }

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
    motion.scale = vec2({ tileSize, tileSize });
    motion.shape = "square";
    
    auto& terrain = ECS::registry<Terrain>.emplace(entity);
    terrain.type = type;
    terrain.friction = friction;

    // Create and (empty) Tile component to be able to refer to all tiles
    auto& tile = ECS::registry<Tile>.emplace(entity);
    tile.splatEntity = ECS::Entity();
    ECS::registry<Motion>.emplace(tile.splatEntity);
    
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

    std::string stringColor = "";
    switch (color) {
        case blobuleCol::Blue:
            stringColor = "blue";
            break;
        case blobuleCol::Green:
            stringColor = "green";
            break;
        case blobuleCol::Red:
            stringColor = "red";
            break;
        default:
            stringColor = "yellow";
            break;
    }

    // guard preventing more splats of the same color from being made on the same tile
    if (colorMap.find(entity.id) != colorMap.end() && colorMap.find(entity.id)->second == stringColor)
    {
        return;
    }
    //std::cout << "making splat" << std::endl;

    colorMap[entity.id] = stringColor;

    auto& tile = ECS::registry<Tile>.get(entity);
    ECS::Entity splatEntity = tile.splatEntity;
    ECS::ContainerInterface::remove_all_components_of(splatEntity);

    std::string key = "splat_";

    key += stringColor;

    switch (color) {
        case blobuleCol::Blue:
            ECS::registry<BlueSplat>.emplace(splatEntity);
            break;
        case blobuleCol::Green:
            ECS::registry<GreenSplat>.emplace(splatEntity);
            break;
        case blobuleCol::Red:
            ECS::registry<RedSplat>.emplace(splatEntity);
            break;
        default:
            ECS::registry<YellowSplat>.emplace(splatEntity);
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

void Tile::setRandomSplat(ECS::Entity entity)
{
    auto& tile = ECS::registry<Tile>.get(entity);
    if (ECS::registry<BlueSplat>.has(tile.splatEntity) || ECS::registry<RedSplat>.has(tile.splatEntity) || ECS::registry<YellowSplat>.has(tile.splatEntity) || ECS::registry<GreenSplat>.has(tile.splatEntity))
    {
        int color = 0 + (std::rand() % (3 - 0 + 1));
        std::string stringColor = "";
        switch (color) {
            case 0:
                stringColor = "blue";
                break;
            case 1:
                stringColor = "green";
                break;
            case 2:
                stringColor = "red";
                break;
            default:
                stringColor = "yellow";
                break;
        }

        // guard preventing more splats of the same color from being made on the same tile
        if (colorMap.find(entity.id) != colorMap.end() && colorMap.find(entity.id)->second == stringColor)
        {
            return;
        }

        colorMap[entity.id] = stringColor;

        ECS::Entity splatEntity = tile.splatEntity;
        ECS::ContainerInterface::remove_all_components_of(splatEntity);

        std::string key = "splat_";
        key += stringColor;

        switch (color) {
            case 0:
                ECS::registry<BlueSplat>.emplace(splatEntity);
                break;
            case 1:
                ECS::registry<GreenSplat>.emplace(splatEntity);
                break;
            case 2:
                ECS::registry<RedSplat>.emplace(splatEntity);
                break;
            default:
                ECS::registry<YellowSplat>.emplace(splatEntity);
                break;
        }

        ShadedMesh& resource = cache_resource(key);
        if (resource.effect.program.resource == 0)
        {
            resource = ShadedMesh();
            std::cout<<key<<std::endl;
            RenderSystem::createSprite(resource, textures_path(key.append(".png")), "textured");

        }
        ECS::registry<ShadedMeshRef>.emplace(splatEntity, resource);

        auto& tileMotion = ECS::registry<Motion>.get(entity);
        auto& splatMotion = ECS::registry<Motion>.emplace(splatEntity);
        splatMotion.position = tileMotion.position;
        splatMotion.scale = vec2({ size * 0.6, size * 0.6 }) * static_cast<vec2>(resource.texture.size);
        splatMotion.isCollidable = false;
    }
}

