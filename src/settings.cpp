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
ECS::Entity background_music_button;
ECS::Entity sound_effects_button;
ECS::Entity background_music_text;
ECS::Entity sound_effects_text;
ECS::Entity save_text;
bool sound_effects_on;
bool background_music_on;

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
	ECS::ContainerInterface::remove_all_components_of(background_music_text);
	ECS::ContainerInterface::remove_all_components_of(sound_effects_text);
	ECS::ContainerInterface::remove_all_components_of(background_music_button);
	ECS::ContainerInterface::remove_all_components_of(sound_effects_button);
	ECS::ContainerInterface::remove_all_components_of(save_text);
}

void enableSoundEffects() {
	if (sound_effects_on) {
		sound_effects_on = false;
		Mix_Volume(-1, 0);
	}
	else {
		sound_effects_on = true;
		Mix_Volume(-1, 100);
	}

	auto sound_effects_str = (sound_effects_on ? "Sound Effects: On" : "Sound Effects: Off");
	auto sound_effect_buttonEnum = (sound_effects_on ? ButtonEnum::SoundOff : ButtonEnum::SoundOn);
	auto sound_effect_pos = ECS::registry<Motion>.get(sound_effects_button).position;
	auto& entity = ECS::registry<Text>.get(sound_effects_text);
	entity.content = sound_effects_str;
	ECS::ContainerInterface::remove_all_components_of(sound_effects_button);
	sound_effects_button = Button::createButton({ sound_effect_pos.x, sound_effect_pos.y}, { 0.77, 0.77 }, sound_effect_buttonEnum, "");
}


void enableBackgroundMusic() {
	if (background_music_on) {
		background_music_on = false;
		Mix_VolumeMusic(0);
	}
	else {
		background_music_on = true;
		Mix_VolumeMusic(100);
	}

	auto background_music_str = (background_music_on ? "Background Music: On" : "Background Music: Off");
	auto background_music_buttonEnum = (background_music_on ? ButtonEnum::SoundOff : ButtonEnum::SoundOn);
	auto background_music_pos = ECS::registry<Motion>.get(background_music_button).position;
	auto& entity = ECS::registry<Text>.get(background_music_text);
	entity.content = background_music_str;
	ECS::ContainerInterface::remove_all_components_of(background_music_button);
	background_music_button = Button::createButton({ background_music_pos.x, background_music_pos.y }, { 0.77, 0.77 }, background_music_buttonEnum, "");
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
	auto& entity = ECS::registry<Text>.get(save_text);
	entity.content = "Save Complete";

}
//public functions
ECS::Entity Settings::createSettings(vec2 position, vec2 scale)
{
	sound_effects_on = (Mix_Volume(-1, -1) != 0);
	background_music_on = (Mix_VolumeMusic(-1) != 0);

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
	
	auto sound_effects_str = (sound_effects_on ? "Sound Effects: On" : "Sound Effects: Off");
	auto background_music_str = (background_music_on ? "Background Music: On" : "Background Music: Off");
	auto sound_effect_buttonEnum = (sound_effects_on ? ButtonEnum::SoundOff : ButtonEnum::SoundOn);
	auto background_music_buttonEnum = (background_music_on ? ButtonEnum::SoundOff : ButtonEnum::SoundOn);

	save_button = Button::createButton({ motion.position.x, motion.position.y - 60}, { 0.50, 0.50 }, ButtonEnum::SaveGame, "Save");
	load_button = Button::createButton({ motion.position.x, motion.position.y + 20}, { 0.50, 0.50 }, ButtonEnum::LoadGame_Settings, "Load");
	restart_button = Button::createButton({ motion.position.x, motion.position.y + 100 }, { 0.50, 0.50 }, ButtonEnum::RestartGame, "Restart");
	quit_button = Button::createButton({ motion.position.x, motion.position.y + 180 }, { 0.50, 0.50 }, ButtonEnum::QuitGame, "Quit");
	exit_button = Button::createButton({ motion.position.x + 425, motion.position.y - 275 }, { 0.40,0.40 }, ButtonEnum::ExitTool, "");
	background_music_button = Button::createButton({ motion.position.x +275, motion.position.y - 190 }, { 0.77,0.77}, background_music_buttonEnum, "");
	sound_effects_button = Button::createButton({ motion.position.x + 275, motion.position.y - 140 }, { 0.77, 0.77 }, sound_effect_buttonEnum, "");
	background_music_text = Text::create_text(background_music_str, {motion.position.x/1.75, motion.position.y - 180 }, 0.5);
	sound_effects_text = Text::create_text(sound_effects_str, { motion.position.x/1.75, motion.position.y - 130 }, 0.5);
	save_text = Text::create_text("", { motion.position.x/1.37, motion.position.y + 255}, 0.5);

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
			else if (PhysicsSystem::is_entity_clicked(sound_effects_button, mouse_x, mouse_y)) {
				enableSoundEffects();
			}
			else if (PhysicsSystem::is_entity_clicked(background_music_button, mouse_x, mouse_y)) {
				enableBackgroundMusic();
			}
}




