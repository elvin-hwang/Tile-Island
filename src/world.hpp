#pragma once

// internal
#include "common.hpp"
#include "blobule.hpp"

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
	ECS::Entity save_button;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	ECS::Entity active_player;
};
