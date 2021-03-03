// Header
#include "helptool.hpp"
#include "render.hpp"

ECS::Entity HelpTool::createHelpTool(vec2 position)
{
    // Reserve an entity
    auto entity = ECS::Entity();

    // Create the rendering components
    std::string key = "help";
    ShadedMesh& resource = cache_resource(key);
    if (resource.effect.program.resource == 0)
    {
        resource = ShadedMesh();
        RenderSystem::createSprite(resource, textures_path("help_tool.png"), "textured");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);

    // Initialize the position, scale and physics components.
    // The only relevant component is position, as the others will not be used.
    auto& motion = ECS::registry<Motion>.emplace(entity);
    motion.position = position;
    motion.scale = vec2({ 0.75f, 0.75f }) * static_cast<vec2>(resource.texture.size);

    ECS::registry<HelpTool>.emplace(entity);

    return entity;
}
