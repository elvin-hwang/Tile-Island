// Header
#include "button.hpp"
#include "render.hpp"

#include <text.hpp>
float buttonFontSize = 0.66;

ECS::Entity Button::createButton(vec2 position, vec2 scale, std::string buttonstring)
{
    // Reserve an entity
    auto entity = ECS::Entity();

    // Create the rendering components
    std::string key = "button" + buttonstring;
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        std::string path = textures_path("blue_button.png");;

        if (buttonstring == "Save") {
            path = textures_path("yellow_button.png");
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

    if (buttonstring == "Save") {
        Text::create_text(buttonstring, { position.x - 40, position.y + 5 }, 0.4);
    }
    else {
        Text::create_text(buttonstring, { position.x - 60, position.y + 10 }, buttonFontSize);
    }

    auto& button = ECS::registry<Button>.emplace(entity);
    return entity;
}


