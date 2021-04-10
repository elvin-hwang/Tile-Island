// Header
#include "button.hpp"
#include "render.hpp"

#include <text.hpp>
float blueButtonFontSize = 0.66;
float yellowButtonFontSize = 0.45;


void createButtonText(Button& button, vec2 position, ButtonEnum buttonEnum, std::string buttonText) {
    switch (buttonEnum) {
    case ButtonEnum::SaveGame:
    case ButtonEnum::LoadGame_Settings:
    case ButtonEnum::QuitGame:
    case ButtonEnum::OpenSettings:
    case ButtonEnum::RestartGame:
        button.text_entity = Text::create_text(buttonText, { position.x - 60, position.y + 5 }, yellowButtonFontSize);
        break;
    default:
        button.text_entity = Text::create_text(buttonText, { position.x - 60, position.y + 10 }, blueButtonFontSize);
        break;
    }
}


ECS::Entity Button::createButton(vec2 position, vec2 scale, ButtonEnum buttonEnum, std::string buttonText)
{
    // Reserve an entity
    auto entity = ECS::Entity();

    // Create the rendering components
    std::string key = "button" + std::to_string(buttonEnum);
    if (buttonEnum == ButtonEnum::LoadMaps) {
        key += buttonText;
    }
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        std::string path;

        switch (buttonEnum) {
            case ButtonEnum::SaveGame:
            case ButtonEnum::LoadGame_Settings:
            case ButtonEnum::QuitGame:
            case ButtonEnum:: RestartGame:
                path = textures_path("yellow_button.png");
                break;
            case ButtonEnum:: ExitTool:
                path = textures_path("exit.png");
                break;
            case ButtonEnum:: OpenSettings:
                path = textures_path("open_settings.png");
                break;
            case ButtonEnum:: SoundOn:
                path = textures_path("sound_on.png");
                break;
            case ButtonEnum:: SoundOff:
                path = textures_path("sound_off.png");
                break;
            case ButtonEnum::OpenHelp:
                path = textures_path("open_help.png");
                break;
            default:
                path = textures_path("blue_button.png");
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
    createButtonText(button, motion.position, buttonEnum, buttonText);

    return entity;
}
