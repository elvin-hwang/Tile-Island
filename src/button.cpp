// Header
#include "button.hpp"
#include "render.hpp"

ECS::Entity Button::createButton(vec2 position, vec2 scale, buttonType type, std::string buttonstring)
{
    // Reserve an entity
    auto entity = ECS::Entity();

    // Create the rendering components
    std::string key = "button" + buttonstring;
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        std::string path;
        switch (type) {
        case buttonType::Start:
            path = textures_path("start_button.png");
            break;
        case buttonType::Load:
            path = textures_path("load_button.png");
            break;
        case buttonType::Save:
            path = textures_path("save_button.png");
            break;
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
    motion.scale = scale * static_cast<vec2>(resource.texture.size);

    auto& button = ECS::registry<Button>.emplace(entity);
    button.buttonEnum = type;
    return entity;
}


