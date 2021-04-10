// Header
#include "blobule.hpp"
#include "render.hpp"

ECS::Entity Blobule::createBlobule(vec2 position, blobuleCol col, std::string colString)
{
    // Reserve an entity 
    auto entity = ECS::Entity();
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
            path = textures_path("yellow_highlight.png");
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

    // Create and (empty) Blobule component to be able to refer to all blobs
    auto& blob = ECS::registry<Blobule>.emplace(entity);
    blob.origin = position;
    blob.color = colString;
    blob.colEnum = col;
    blob.trajectoryEntity = blob.createTrajectory(blob);
    return entity;
}

void Blobule::setTrajectory(ECS::Entity entity) {
    if (!ECS::registry<Blobule>.has(entity)) {
        return;
    }
    auto& blob = ECS::registry<Blobule>.get(entity);
    auto& trajMesh = *ECS::registry<ShadedMeshRef>.get(blob.trajectoryEntity).reference_to_cache;

    auto& blobMotion = ECS::registry<Motion>.get(entity);
    auto& trajMotion = ECS::registry<Motion>.get(blob.trajectoryEntity);

    float powerScale = min(abs(blobMotion.dragDistance) * 0.01, 3.5);
    if (powerScale < 0.2) {
        powerScale = 0;
    }

    trajMotion.angle = blobMotion.angle + PI / 2;
    trajMotion.scale = static_cast<vec2>(trajMesh.texture.size) * vec2{ 1.f, sqrt(powerScale) };
    trajMotion.position = blobMotion.position + vec2{ cos(blobMotion.angle) * (blobMotion.scale.x / 2 + trajMotion.scale.y / 2), sin(blobMotion.angle)* (blobMotion.scale.x / 2 + trajMotion.scale.y / 2) };
    trajMotion.isCollidable = false;
}

void Blobule::removeTrajectory(ECS::Entity entity) {
    if (!ECS::registry<Blobule>.has(entity)) {
        return;
    }
    auto& blob = ECS::registry<Blobule>.get(entity);
    auto& trajMotion = ECS::registry<Motion>.get(blob.trajectoryEntity);
    trajMotion.scale = { 0.f, 0.f };
}

ECS::Entity Blobule::createTrajectory(Blobule& blob) {
    ECS::Entity entity = ECS::Entity();

    ShadedMesh& resource = cache_resource("dotted_line");
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("dotted_line.png"), "textured");

    }
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);

    // Change color of dotted line based on enum?
    switch (blob.colEnum) {
    case blobuleCol::Blue:
        break;
    case blobuleCol::Green:
        break;
    case blobuleCol::Red:
        break;
    case blobuleCol::Yellow:
        break;
    default:
        break;
    }

    auto& trajectoryMotion = ECS::registry<Motion>.emplace(entity);
    trajectoryMotion.scale = { 0.f, 0.f };
    trajectoryMotion.isCollidable = false;

    return entity;
}

