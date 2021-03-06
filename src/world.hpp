#pragma once

// internal
#include "common.hpp"
#include "blobule.hpp"
#include "level_editor.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	// Creates a window
	WorldSystem(ivec2 window_size_px);

	// Releases all associated resources
	~WorldSystem();

	// restart level
	void restart();

	// Steps the game ahead by ms milliseconds
	void step(float elapsed_ms, vec2 window_size_in_game_units);

	// Check for collisions
	void handle_collisions();

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over() const;

	// Different states for the world system
	GameState gameState;

	// OpenGL window handle
	GLFWwindow* window;

	static void enable_settings(bool enable);
	static void enable_help(bool enable);
	static void go_to_main_menu();
	static void set_game_to_restart();
	static bool get_blobule_moved();
	static int get_current_turn();
	static int get_player_move();
	static void set_load_map_location(std::string loc);

private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 mouse_pos);
	void on_mouse_button(GLFWwindow* wnd, int button, int action);

	// Loads the audio
	void init_audio();

	// Game state
	float current_speed;

    // Music References
    Mix_Music* background_music;
    Mix_Chunk* slingshot_pull_sound;
    Mix_Chunk* slingshot_shot_sound;
    Mix_Chunk* blobule_yipee_sound;
    Mix_Chunk* game_start_sound;

	ECS::Entity score_text;
	ECS::Entity player_text;
	ECS::Entity end_turn_text;

	ECS::Entity start_button;
	ECS::Entity load_button;
	ECS::Entity level_editor_button;
	ECS::Entity quit_button;
	ECS::Entity settings_button;
	ECS::Entity help_button;
	ECS::Entity save_button;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	ECS::Entity active_player;

	ECS::Entity camera;

	// Level editor
	LevelEditor::EditorEntity selected_editor_entity = LevelEditor::EditorEntity::Sand;
	ECS::Entity editor_save_button;
	ECS::Entity editor_home_button;
	ECS::Entity editor_water;
	ECS::Entity editor_block;
	ECS::Entity editor_ice;
	ECS::Entity editor_mud;
	ECS::Entity editor_sand;
	ECS::Entity editor_acid;
	ECS::Entity editor_speed;
	ECS::Entity editor_speed_UP;
	ECS::Entity editor_speed_LEFT;
	ECS::Entity editor_speed_RIGHT;
	ECS::Entity editor_speed_DOWN;
	ECS::Entity editor_teleport;
	ECS::Entity editor_yellow_blob;
	ECS::Entity editor_green_blob;
	ECS::Entity editor_red_blob;
	ECS::Entity editor_blue_blob;
	ECS::Entity editor_egg;
};
