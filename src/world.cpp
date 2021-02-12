// Header
#include "world.hpp"
#include "physics.hpp"
#include "debug.hpp"
#include "render_components.hpp"
#include "tile.hpp"
#include "blobule.hpp"
#include "wall.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>
#include <egg.hpp>

// Game Configuration

// Position of first tile.
// Should figure out a way to position this such that the grid will always be centered on the background.
float first_loc_x = 78.f;
float first_loc_y = 90.f;
float grid_width_x = 845.f;
float grid_width_y = 620.f;


// Movement speed of blobule.
float moveSpeed = 100.f;
float terminalVelocity = 20.f;

double mouse_press_x, mouse_press_y;

// Set the width and height of grid in terms of number of tiles.
static const int grid_width = 20;
static const int grid_height = 15;
int grid_size = grid_width * grid_height;

int playerMove = 1;

// We need to store a an array of vec2 that contains the locations of every tile on the grid.
// For example, perhaps index 3 of the array contains {100, 200}, meaning a tile exists on 100, 200.
vec2 GRID[grid_width * grid_height];

// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.
WorldSystem::WorldSystem(ivec2 window_size_px)
{
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());

	// Initialize GLFW
	auto glfw_err_callback = [](int error, const char* desc) { std::cerr << "OpenGL:" << error << desc << std::endl; };
	glfwSetErrorCallback(glfw_err_callback);
	if (!glfwInit())
		throw std::runtime_error("Failed to initialize GLFW");

	//-------------------------------------------------------------------------
	// GLFW / OGL Initialization, needs to be set before glfwCreateWindow
	// Core Opengl 3.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(window_size_px.x, window_size_px.y, "Tile Island", nullptr, nullptr);
	if (window == nullptr)
		throw std::runtime_error("Failed to glfwCreateWindow");

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_move({ _0, _1 }); };
	auto mouse_button_callback = [](GLFWwindow* wnd, int _button, int _action, int _mods) { ((WorldSystem*)glfwGetWindowUserPointer(wnd))->on_mouse_button(wnd, _button, _action); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
}

WorldSystem::~WorldSystem(){
	Mix_CloseAudio();

	// Destroy all created components
	ECS::ContainerInterface::clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Loading music and sounds with SDL
void WorldSystem::init_audio()
{
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
		throw std::runtime_error("Failed to initialize SDL Audio");

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
		throw std::runtime_error("Failed to open audio device");
}

// Update our game world
void WorldSystem::step(float elapsed_ms, vec2 window_size_in_game_units)
{
	(void)elapsed_ms; // silence unused warning
	(void)window_size_in_game_units; // silence unused warning
    
    // Giving our game a title.
    std::stringstream title_ss;
    title_ss << "Welcome to Tile Island!";;
    glfwSetWindowTitle(window, title_ss.str().c_str());

	// Friction implementation
	for (auto& blob : ECS::registry<Blobule>.entities)
	{
		auto& motion = ECS::registry<Motion>.get(blob);
		motion.velocity += -motion.velocity * motion.friction;
		if (-terminalVelocity < motion.velocity.x && motion.velocity.x < terminalVelocity) {
			motion.velocity.x = 0;
		}
		if (-terminalVelocity < motion.velocity.y && motion.velocity.y < terminalVelocity) {
			motion.velocity.y = 0;
		}
	}
}

// Reset the world state to its initial state
void WorldSystem::restart()
{
	// Debugging for memory/component leaks
	ECS::ContainerInterface::list_all_components();
	std::cout << "Restarting\n";

	// Reset the game speed
	current_speed = 1.f;

	// Remove all entities that we created (those that have a motion component)
	while (ECS::registry<Motion>.entities.size()>0)
		ECS::ContainerInterface::remove_all_components_of(ECS::registry<Motion>.entities.back());

	// Debugging for memory/component leaks
	ECS::ContainerInterface::list_all_components();
    
    // Generate our default grid first.
    // We will place tiles such that they form a 5 x 8 grid. Each tile will be placed next to one another.
    
	Tile::createWaterTile({ 0, 0 });

    // Make one tile at the origin of the grid first.
    ECS::Entity entity_tile = Tile::createBlueTile({first_loc_x, first_loc_y});
    
    // Make a 20 x 15 Grid of Tiles.
    // First, get the dimensions of one tile defined in tile.cpp.
    auto& motion = ECS::registry<Motion>.get(entity_tile);
    auto width = motion.scale.x;
    auto height = motion.scale.y;
    int count = 0;
    // Horizontally...
    for (int i = 0; i < grid_width; i++)
    {
        // Vertically...
        for (int j = 0; j < grid_height; j++)
        {
            // Calculate position of tile to be generated.
            float loc_x = first_loc_x + (width * i);
            float loc_y = first_loc_y + (height * j);
            // Place locations in GRID.
            vec2 new_location_for_tile = {loc_x, loc_y};
            GRID[count] = new_location_for_tile;
            count++;
            
            // Create a tile everywhere on half of the grid.
			if (i < grid_width / 2) {
				Tile::createBlueTile({ loc_x, loc_y });
			} else {
				Tile::createPurpleTile({ loc_x, loc_y });
			}
        }
    }
    
    // Create blobule characters
	if (ECS::registry<Blobule>.components.size() <= 4) {
		player_blobule1 = Blobule::createBlobule({ first_loc_x, first_loc_y }, blobuleCol::Yellow, "yellow");
		player_blobule2 = Blobule::createBlobule({ first_loc_x + grid_width_x, first_loc_y }, blobuleCol::Green, "green");
		player_blobule3 = Blobule::createBlobule({ first_loc_x, first_loc_y + grid_width_y }, blobuleCol::Red, "red");
		player_blobule4 = Blobule::createBlobule({ first_loc_x + grid_width_x, first_loc_y + grid_width_y }, blobuleCol::Blue, "blue");
		active_player = player_blobule1;
	}

	//Only one npc for now
	if (ECS::registry<Egg>.components.size() < 1)
	{
		// Create egg
		ECS::Entity entity = Egg::createEgg({ first_loc_x + grid_width_x/2, first_loc_x + grid_width_y/2 });
		//add movement things here 
	}

	// Create walls (hardcoded for now) 
	ECS::Entity wall = Wall::createWall("wall", { 400.f, 400.f }, 0.f);
	auto &wall_motion = ECS::registry<Motion>.get(wall);
	auto wall_height = wall_motion.scale.y;
	auto wall_width = wall_motion.scale.x;
	ECS::Entity wall_2 = Wall::createWall("wall_corner", { 400.f, 400.f + wall_height }, 0.f);
	ECS::Entity wall_3 = Wall::createWall("wall_end", { 400.f + wall_width, 400.f + wall_height }, -PI / 2);
}

// Should the game be over ?
bool WorldSystem::is_over() const
{
	return glfwWindowShouldClose(window)>0;
}

// On key callback
// Check out https://www.glfw.org/docs/3.3/input_guide.html
void WorldSystem::on_key(int key, int, int action, int mod)
{
	switch (playerMove) {
		case 1:
			active_player = player_blobule1;
			break;
		case 2:
			active_player = player_blobule2;
			break;
		case 3:
			active_player = player_blobule3;
			break;
		case 4:
			active_player = player_blobule4;
			break;
	}

	auto& blobule_movement = ECS::registry<Motion>.get(active_player);
    auto blobule_position = blobule_movement.position;
            
    // For when you press an arrow key and the salmon starts moving.
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_UP)
        {
            // Note: Subtraction causes upwards movement.
            blobule_movement.velocity.y = -moveSpeed;
        }
        if (key == GLFW_KEY_DOWN)
        {
            // Note: Addition causes downwards movement.
            blobule_movement.velocity.y = moveSpeed;
        }
        if (key == GLFW_KEY_LEFT)
        {
            blobule_movement.velocity.x = -moveSpeed;
                    
        }
        if (key == GLFW_KEY_RIGHT)
        {
            blobule_movement.velocity.x = moveSpeed;
                    
        }
    }

	// Turn based system
	if (action == GLFW_PRESS && key == GLFW_KEY_ENTER)
	{
		if (playerMove != 4) {
			playerMove++;
		}
		else {
			playerMove = 1;
		}
	}
    
	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		int w, h;
		glfwGetWindowSize(window, &w, &h);

		restart();
	}

	// Debugging
	if (key == GLFW_KEY_D)
		DebugSystem::in_debug_mode = (action != GLFW_RELEASE);

	// Control the current speed with `<` `>`
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA)
	{
		current_speed -= 0.1f;
		std::cout << "Current speed = " << current_speed << std::endl;
	}
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD)
	{
		current_speed += 0.1f;
		std::cout << "Current speed = " << current_speed << std::endl;
	}
	current_speed = std::max(0.f, current_speed);
}

// On mouse move callback
void WorldSystem::on_mouse_move(vec2 mouse_pos)
{
    (void)mouse_pos;
}

// On mouse button callback
void WorldSystem::on_mouse_button(GLFWwindow* wnd, int button, int action)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        glfwGetCursorPos(wnd, &mouse_press_x, &mouse_press_y);
        ECS::registry<Motion>.get(active_player).angle = atan2(mouse_press_y - ECS::registry<Motion>.get(active_player).position.y, mouse_press_x - ECS::registry<Motion>.get(active_player).position.x) - PI;
    }

    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        double mouse_release_x, mouse_release_y;
        glfwGetCursorPos(wnd, &mouse_release_x, &mouse_release_y);
        double drag_distance = (((mouse_release_y - mouse_press_y) * (mouse_release_y - mouse_press_y)) + ((mouse_release_x - mouse_press_x) * (mouse_release_x - mouse_press_x))) * 0.01;
        ECS::registry<Motion>.get(active_player).velocity = {cos(ECS::registry<Motion>.get(active_player).angle) * drag_distance, sin(ECS::registry<Motion>.get(active_player).angle) * drag_distance};
    }
}
