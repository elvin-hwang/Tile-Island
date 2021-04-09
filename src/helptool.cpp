// Header
#include "helptool.hpp"
#include "render.hpp"
#include <physics.hpp>
#include <button.hpp>
#include <world.hpp>
ECS::Entity exit_help;

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
    motion.scale = vec2({ 0.90f, 0.75f }) * static_cast<vec2>(resource.texture.size);

    ECS::registry<HelpTool>.emplace(entity);

    exit_help = Button::createButton({ motion.position.x*1.86, motion.position.y/3.5 }, { 0.41,0.41 }, ButtonEnum::ExitTool, "");

    return entity;
}


void HelpTool::handleHelpToolClicks(double mouse_x, double mouse_y)
{
	if (PhysicsSystem::is_entity_clicked(exit_help, mouse_x, mouse_y)) {
        ECS::ContainerInterface::remove_all_components_of(exit_help);
        WorldSystem::enable_help(false);
	}
}