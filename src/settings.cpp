// Header
#include "settings.hpp"
#include "render.hpp"
#include <SDL_mixer.h>
#include <button.hpp>
#include <physics.hpp>
#include <world.hpp>
#include <text.hpp>
#include <map_loader.hpp>

ECS::Entity save_button;
ECS::Entity load_button;
ECS::Entity exit_button;
ECS::Entity quit_button;
ECS::Entity restart_button;

//private functions
void closeSettings() {
	ECS::ContainerInterface::remove_all_components_of(ECS::registry<Button>.get(save_button).text_entity);
	ECS::ContainerInterface::remove_all_components_of(ECS::registry<Button>.get(load_button).text_entity);
	ECS::ContainerInterface::remove_all_components_of(ECS::registry<Button>.get(quit_button).text_entity);
	ECS::ContainerInterface::remove_all_components_of(ECS::registry<Button>.get(restart_button).text_entity);
	ECS::ContainerInterface::remove_all_components_of(save_button);
	ECS::ContainerInterface::remove_all_components_of(load_button);
	ECS::ContainerInterface::remove_all_components_of(exit_button);
	ECS::ContainerInterface::remove_all_components_of(quit_button);
	ECS::ContainerInterface::remove_all_components_of(restart_button);
}

void muteVolume() {
	Mix_VolumeMusic(0);
}

void saveGame() {
	int currPlayer = WorldSystem::get_player_move();
	if (WorldSystem::get_blobule_moved()) {
		currPlayer++;
		if (currPlayer > 3) {
			currPlayer = 0;
		}
	}
	MapLoader::saveMap(currPlayer, WorldSystem::get_current_turn());
}
//public functions
ECS::Entity Settings::createSettings(vec2 position, vec2 scale)
{
	// Reserve an entity
	auto entity = ECS::Entity();

	// Create the rendering components
	std::string key = "settings";
	ShadedMesh& resource = cache_resource(key);
	if (resource.effect.program.resource == 0)
	{
		resource = ShadedMesh();
		std::string path = textures_path("settings.png");;

		RenderSystem::createSprite(resource, path, "textured");
	}
	ECS::registry<ShadedMeshRef>.emplace(entity, resource);

	auto& motion = ECS::registry<Motion>.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;
	motion.friction = 0.f;
	motion.scale = scale * static_cast<vec2>(resource.texture.size);
	save_button = Button::createButton({ motion.position.x, motion.position.y + 20 }, { 0.50, 0.50 }, ButtonEnum::SaveGame, "Save");
	load_button = Button::createButton({ motion.position.x, motion.position.y + 100 }, { 0.50, 0.50 }, ButtonEnum::LoadGame_Settings, "Load");
	restart_button = Button::createButton({ motion.position.x, motion.position.y + 180 }, { 0.50, 0.50 }, ButtonEnum::RestartGame, "Restart");
	quit_button = Button::createButton({ motion.position.x, motion.position.y + 260 }, { 0.50, 0.50 }, ButtonEnum::QuitGame, "Quit");
	exit_button = Button::createButton({ motion.position.x + 425, motion.position.y - 275 }, { 0.40,0.40 }, ButtonEnum::ExitSettings, "");
	auto& settings = ECS::registry<Settings>.emplace(entity);

	return entity;
}

void Settings::handleSettingClicks(double mouse_x, double mouse_y)
{
	if (PhysicsSystem::is_entity_clicked(save_button, mouse_x, mouse_y)) {
		saveGame();
	}
	else
		if (PhysicsSystem::is_entity_clicked(exit_button, mouse_x, mouse_y)) {
			WorldSystem::enable_settings(false);
			closeSettings();
		}
		else
			if (PhysicsSystem::is_entity_clicked(quit_button, mouse_x, mouse_y)) {
				WorldSystem::quit_game();
			}
			else if (PhysicsSystem::is_entity_clicked(restart_button, mouse_x, mouse_y)) {
				WorldSystem::set_game_to_restart();
			}
			else if (PhysicsSystem::is_entity_clicked(load_button, mouse_x, mouse_y)) {
				WorldSystem::set_load_map_location("data/saved/map.json");
				WorldSystem::set_game_to_restart();
			}
}




