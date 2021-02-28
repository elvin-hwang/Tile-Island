#pragma once

// internal
#include "common.hpp"
#include "blobule.hpp"
#include "collisions.hpp"

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
	void step(float elapsed_ms, vec2 window_size_in_game_units,
              unsigned int yellow_tiles,
              unsigned int green_tiles,
              unsigned int red_tiles,
              unsigned int blue_tiles);

	// Check for collisions
	void handle_collisions();

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over() const;

	bool menuState;
    bool restarted;

	// OpenGL window handle
	GLFWwindow* window;
    
    // Anything related to counting coloured tiles
    unsigned int yellow_tiles = 0;
    unsigned int green_tiles = 0;
    unsigned int red_tiles = 0;
    unsigned int blue_tiles = 0;
    
private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 mouse_pos);
	void on_mouse_button(GLFWwindow* wnd, int button, int action);

	// Loads the audio
	void init_audio();

	// Game state
	float current_speed;
    ECS::Entity player_blobule1;
	ECS::Entity player_blobule2;
	ECS::Entity player_blobule3;
	ECS::Entity player_blobule4;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1

	ECS::Entity active_player;
};
