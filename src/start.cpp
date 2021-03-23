// Header
#include "start.hpp"
#include "render.hpp"

ECS::Entity Menu::createMenu(vec2 position, GameState gameState)
{
    // Reserve an entity
    auto entity = ECS::Entity();

    // Create the rendering components

    /*
    Setting this as empty string to force a fresh fetch of the texture based on gameState.
    Shouldn't affect performance significantly since we're seeing this textures very few times.
    */
    std::string key = "";

    ShadedMesh& resource = cache_resource(key);
    resource = ShadedMesh();

    switch (gameState) {
    case GameState::Start:
        RenderSystem::createSprite(resource, textures_path("start_menu.png"), "textured");
        break;
    case GameState::Intro:
        RenderSystem::createSprite(resource, textures_path("story_1.png"), "textured");
        break;
    case GameState::Yellow:
        RenderSystem::createSprite(resource, textures_path("story_2.png"), "textured");
        break;
    case GameState::Green:
        RenderSystem::createSprite(resource, textures_path("story_3.png"), "textured");
        break;
    case GameState::Red:
        RenderSystem::createSprite(resource, textures_path("story_4.png"), "textured");
        break;
    case GameState::Blue:
        RenderSystem::createSprite(resource, textures_path("story_5.png"), "textured");
        break;
    case GameState::Paint:
        RenderSystem::createSprite(resource, textures_path("story_6.png"), "textured");
        break;
    case GameState::Island:
        RenderSystem::createSprite(resource, textures_path("story_7.png"), "textured");
        break;
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);

    // Initialize the position, scale and physics components.
    // The only relevant component is position, as the others will not be used.
    auto& motion = ECS::registry<Motion>.emplace(entity);
    motion.position = position;
    motion.scale = (gameState == GameState::Start ? vec2({ 0.65f, 0.7f }) : vec2(1.f, 1.f)) * static_cast<vec2>(resource.texture.size);

    return entity;
}
