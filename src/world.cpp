// Header
#include "world.hpp"
#include "physics.hpp"
#include "debug.hpp"
#include "render_components.hpp"
#include "tile.hpp"
#include "blobule.hpp"


// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>

// Game Configuration

// Position of first tile.
// Should figure out a way to position this such that the grid will always be centered on the background.
float first_loc_x = 142.f;
float first_loc_y = 130.f;

// Movement size of blobule.
float move = 103.f;

// Set the width and height of grid in terms of number of tiles.
static const int grid_width = 8;
static const int grid_height = 6;
int grid_size = grid_width * grid_height;

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
    
    // Make one tile at the origin of the grid first.
    ECS::Entity entity_tile = Tile::createBlueTile({first_loc_x, first_loc_y});
    
    // Make a 8 x 5 Grid of Tiles.
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
            
            // Create a tile everywhere except the origin of the grid.
            if (!(i == 0 && j == 0)){
                Tile::createPurpleTile({loc_x, loc_y});
            }
        }
    }
    
    // Create and place our blobule at the origin of the grid.
    player_blobule = Blobule::createBlobule({first_loc_x, first_loc_y});
}

// Compute collisions between entities
void WorldSystem::handle_collisions()
{
	// Loop over all collisions detected by the physics system
	auto& registry = ECS::registry<PhysicsSystem::Collision>;
	for (unsigned int i=0; i< registry.components.size(); i++)
	{
		// The entity and its collider
		auto entity = registry.entities[i];
		auto entity_other = registry.components[i].other;
	}

	// Remove all collisions from this simulation step
	ECS::registry<PhysicsSystem::Collision>.clear();
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
    // Retrieve player salmon Motion data.
    auto& blobule_movement = ECS::registry<Motion>.get(player_blobule);
    auto blobule_position = blobule_movement.position;
            
    // For when you press an arrow key and the salmon starts moving.
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_UP)
        {
            // Note: Subtraction causes upwards movement.
            blobule_movement.position = { blobule_position.x, blobule_position.y - move};
        }
        if (key == GLFW_KEY_DOWN)
        {
            // Note: Addition causes downwards movement.
            blobule_movement.position = { blobule_position.x, blobule_position.y + move};
        }
        if (key == GLFW_KEY_LEFT)
        {
            blobule_movement.position = { blobule_position.x - move, blobule_position.y};
                    
        }
        if (key == GLFW_KEY_RIGHT)
        {
            blobule_movement.position = { blobule_position.x + move, blobule_position.y};
                    
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
        double mouse_x, mouse_y;
        glfwGetCursorPos(wnd, &mouse_x, &mouse_y);
        ECS::registry<Motion>.get(player_blobule).angle = atan2(mouse_y - ECS::registry<Motion>.get(player_blobule).position.y, mouse_x - ECS::registry<Motion>.get(player_blobule).position.x) - PI;
    }

    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        ECS::registry<Motion>.get(player_blobule).velocity = {cos(ECS::registry<Motion>.get(player_blobule).angle) * 50, sin(ECS::registry<Motion>.get(player_blobule).angle) * 50};
    }
}
