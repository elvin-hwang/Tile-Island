// Header
#include "world.hpp"
#include "physics.hpp"
#include "debug.hpp"
#include "render_components.hpp"
#include "tile.hpp"
#include "blobule.hpp"
#include "start.hpp"
#include "helptool.hpp"
#include "collisions.hpp"
#include "utils.hpp"
#include "map_loader.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>
#include <egg.hpp>
#include <text.hpp>
#include <button.hpp>


// Game Configuration

// Tile Configurations
int numWidth = 0;
int numHeight = 0;
std::vector<std::vector<ECS::Entity>> islandGrid;

// Movement speed of blobule.
float moveSpeed = 200.f;
float terminalVelocity = 20.f;
float max_blobule_speed = 250.f;
vec2 window_size;
ECS::Entity help_tool;

double mouse_press_x, mouse_press_y;

int playerMove = 0;
bool blobuleMoved = false;
bool mouse_move = false;
bool load_game = false;

int current_turn = 0;
int MAX_TURNS = 20;
int next_egg_spawn = 3;
int MAX_EGGS = 1;

// Note, this has a lot of OpenGL specific things, could be moved to the renderer; but it also defines the callbacks to the mouse and keyboard. That is why it is called here.
WorldSystem::WorldSystem(ivec2 window_size_px)
{
	menuState = true;
	window_size = window_size_px;
	playerMove = 0;

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

	// Playing background music indefinitely
	init_audio();
	Mix_PlayMusic(background_music, -1);
	std::cout << "Loaded music\n";
}

WorldSystem::~WorldSystem() {
	Mix_CloseAudio();
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (slingshot_pull_sound != nullptr)
		Mix_FreeChunk(slingshot_pull_sound);
	if (slingshot_shot_sound != nullptr)
		Mix_FreeChunk(slingshot_shot_sound);
	if (blobule_yipee_sound != nullptr)
		Mix_FreeChunk(blobule_yipee_sound);
	if (game_start_sound != nullptr)
		Mix_FreeChunk(game_start_sound);

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

	background_music = Mix_LoadMUS(audio_path("music.wav").c_str());
	slingshot_pull_sound = Mix_LoadWAV(audio_path("slingshot_pull.wav").c_str());
	slingshot_shot_sound = Mix_LoadWAV(audio_path("slingshot_shot.wav").c_str());
	blobule_yipee_sound = Mix_LoadWAV(audio_path("blobule_yipee.wav").c_str());
	game_start_sound = Mix_LoadWAV(audio_path("game_start.wav").c_str());

	if (background_music == nullptr || slingshot_pull_sound == nullptr || slingshot_shot_sound == nullptr || blobule_yipee_sound == nullptr || game_start_sound == nullptr)
		throw std::runtime_error("Failed to load sounds make sure the data directory is present: " +
			audio_path("music.wav") +
			audio_path("slingshot_pull.wav") +
			audio_path("slingshot_shot.wav") +
			audio_path("blobule_yipee.wav") +
			audio_path("game_start.wav"));
}

// Update our game world
void WorldSystem::step(float elapsed_ms, vec2 window_size_in_game_units)
{
    (void)elapsed_ms; // silence unused warning
    (void)window_size_in_game_units; // silence unused warning
    if (!menuState) {
        std::string active_colour = "";
        if (ECS::registry<Blobule>.has(active_player)) {
            active_colour = ECS::registry<Blobule>.get(active_player).color;
            active_colour[0] = toupper(active_colour[0]);
        }

        // Giving our game a title.
        std::stringstream title_ss;
        title_ss << "Welcome to Tile Island!";
        glfwSetWindowTitle(window, title_ss.str().c_str());

        // Updating Score UI
        std::stringstream scores;
        std::stringstream current_player;
        std::string winner_colour = "Blue";

        if (current_turn == MAX_TURNS)
        {
            if (ECS::registry<YellowSplat>.entities.size() >= ECS::registry<GreenSplat>.entities.size() && ECS::registry<YellowSplat>.entities.size() >= ECS::registry<RedSplat>.entities.size() && ECS::registry<YellowSplat>.entities.size() >= ECS::registry<BlueSplat>.entities.size())
            {
                winner_colour = "Yellow";
            }

            else if (ECS::registry<GreenSplat>.entities.size() >= ECS::registry<YellowSplat>.entities.size() && ECS::registry<GreenSplat>.entities.size() >= ECS::registry<RedSplat>.entities.size() && ECS::registry<GreenSplat>.entities.size() >= ECS::registry<BlueSplat>.entities.size())
            {
                winner_colour = "Green";
            }

            else if (ECS::registry<RedSplat>.entities.size() >= ECS::registry<YellowSplat>.entities.size() && ECS::registry<RedSplat>.entities.size() >= ECS::registry<GreenSplat>.entities.size() && ECS::registry<RedSplat>.entities.size() >= ECS::registry<BlueSplat>.entities.size())
            {
                winner_colour = "Red";
            }
        }

        if (ECS::registry<Egg>.components.size() < MAX_EGGS && next_egg_spawn == 0)
        {
            next_egg_spawn = 3;
            auto& motion = ECS::registry<Motion>.get(islandGrid[numWidth / 2][numHeight / 2]);
            ECS::Entity entity = Egg::createEgg(motion.position);
        }

        scores <<
            "Yellow: " << ECS::registry<YellowSplat>.entities.size() <<
            " Green: " << ECS::registry<GreenSplat>.entities.size() <<
            " Red: " << ECS::registry<RedSplat>.entities.size() <<
            " Blue: " << ECS::registry<BlueSplat>.entities.size();
        current_turn == MAX_TURNS ? current_player << "And the winner is: " << winner_colour << "!" : current_player << "Current Player: " << active_colour << " Round: " << 1 + current_turn / 4;

        if (ECS::registry<Text>.size() > 0) {
            ECS::registry<Text>.get(score_text).content = scores.str();
            ECS::registry<Text>.get(player_text).content = current_player.str();
        }

        // Friction implementation
        for (auto& blob : ECS::registry<Blobule>.entities)
        {
            auto& motion = ECS::registry<Motion>.get(blob);
            motion.velocity += -motion.velocity * motion.friction;

            float velocityMagnitude = Utils::getVelocityMagnitude(motion);
            if (velocityMagnitude < terminalVelocity) {
                motion.velocity = { 0.f, 0.f };
            }
        }
    }
}

// Reset the world state to its initial state
void WorldSystem::restart() {
    // Generate our default grid first.
    int window_width, window_height;
    glfwGetWindowSize(window, &window_width, &window_height);

	if (menuState) {
		Menu::createMenu({ window_width / 2, window_height / 2 });
		start_button = Button::createButton({ window_width / 2, window_height / 2 }, { 0.75,0.75 }, buttonType::Start, "start");
		load_button = Button::createButton({ window_width / 2, window_height / 2 + 100 }, { 0.75,0.75 }, buttonType::Load, "load");
	}
    else {
        // Debugging for memory/component leaks
        //ECS::ContainerInterface::list_all_components();

        std::cout << "Restarting\n";


        // Reset other stuff
        playerMove = 0;
        blobuleMoved = false;
        mouse_move = false;
        current_turn = 0;
        MAX_TURNS = 20;

        // Reset the game speed
        current_speed = 1.f;

        // Remove all entities that we created (those that have a motion component)
        while (ECS::registry<Motion>.entities.size() > 0) 
            ECS::ContainerInterface::remove_all_components_of(ECS::registry<Motion>.entities.back());

        while (ECS::registry<ShadedMeshRef>.entities.size() > 0)
            ECS::ContainerInterface::remove_all_components_of(ECS::registry<ShadedMeshRef>.entities.back());

        // Debugging for memory/component leaks
        ECS::ContainerInterface::list_all_components();

        // Can replace loadMap with loadSavedMap
        islandGrid = load_game ? MapLoader::loadSavedMap({ window_width, window_height }) : MapLoader::loadMap("data/level/map_1.json", { window_width, window_height });
        numHeight = islandGrid.size();
        numWidth = islandGrid[0].size();

        // Set initial values
        auto vals = MapLoader::getInitialInfo();
        current_turn = vals[1];
        playerMove = vals[0];
        active_player = MapLoader::getBlobule(playerMove);
        ECS::registry<Blobule>.get(active_player).active_player = true;

        //Create Text
        if (ECS::registry<Text>.components.size() > 0){
            ECS::registry<Text>.clear();
        }

        score_text = Text::create_text("score", { 82, 60 }, 0.58);
        player_text = Text::create_text("player", { 82, 30 }, 0.58);
        save_button = Button::createButton({177, 730}, { 0.35,0.35 }, buttonType::Save, "save");

    }
}

// Should the game be over ?
bool WorldSystem::is_over() const
{
	return glfwWindowShouldClose(window) > 0;
}

// On key callback
// Check out https://www.glfw.org/docs/3.3/input_guide.html
void WorldSystem::on_key(int key, int, int action, int mod)
{
    if(!menuState)
    {
        ECS::registry<Blobule>.get(active_player).active_player = false;
        active_player = MapLoader::getBlobule(playerMove);

        ECS::registry<Blobule>.get(active_player).active_player = true;
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

        if (key == GLFW_KEY_H) {
            if (action == GLFW_PRESS) {
                help_tool = HelpTool :: createHelpTool({ window_size.x / 2, window_size.y / 2 });
            }
            if (action == GLFW_RELEASE) {
                ECS::ContainerInterface::remove_all_components_of(help_tool);
            }
        }


        // For when you press a WASD key and the camera starts moving.
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            // Note that we don't update the tileIsland grid in this function to save performance
            // If we need to, then we can store a global variable for the x,y offsets and use it accordingly.
            float xOffset = 0;
            float yOffset = 0;
            switch (key) {
                case GLFW_KEY_W:
                    yOffset = 10.f;
                    break;
                case GLFW_KEY_S:
                    yOffset = -10.f;
                    break;
                case GLFW_KEY_A:
                    xOffset = 10.f;
                    break;
                case GLFW_KEY_D:
                    xOffset = -10.f;
                    break;
                default:
                    break;
            }
            Utils::moveCamera(xOffset, yOffset);
        }

        // Turn based system
        if (action == GLFW_PRESS && key == GLFW_KEY_ENTER && current_turn < MAX_TURNS)
        {
            if (playerMove != 3) {
                playerMove++;
                current_turn++;
            }
            else {
                playerMove = 0;
                current_turn++;
            }

            if (ECS::registry<Egg>.components.size() < MAX_EGGS)
            {
                next_egg_spawn--;
                if (next_egg_spawn < 0)
                    next_egg_spawn = 0;
                std::cout << next_egg_spawn << std::endl;
            }

            blobuleMoved = false;
        }

        // Resetting game
        if (action == GLFW_RELEASE && key == GLFW_KEY_R)
        {
            int w, h;
            glfwGetWindowSize(window, &w, &h);
            Mix_PlayChannel(-1, game_start_sound, 0);
            load_game = false;
            restart();
        }

        // Debugging
        if (key == GLFW_KEY_Q)
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
}

// On mouse move callback
void WorldSystem::on_mouse_move(vec2 mouse_pos)
{
	if (ECS::registry<Blobule>.has(active_player) && mouse_move)
	{
		auto& blobMotion = ECS::registry<Motion>.get(active_player);
		blobMotion.angle = atan2(mouse_pos.y - mouse_press_y, mouse_pos.x - mouse_press_x) - PI;
		blobMotion.dragDistance = (((mouse_pos.y - mouse_press_y) * (mouse_pos.y - mouse_press_y)) + ((mouse_pos.x - mouse_press_x) * (mouse_pos.x - mouse_press_x))) * 0.01;
		Blobule::setTrajectory(active_player);
	}
	(void)mouse_pos;
}

// On mouse button callback
void WorldSystem::on_mouse_button(GLFWwindow* wnd, int button, int action)
{
	glfwGetCursorPos(wnd, &mouse_press_x, &mouse_press_y);
	if (menuState) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            auto start_clicked = PhysicsSystem::is_entity_clicked(start_button, mouse_press_x, mouse_press_y);
            auto load_clicked = PhysicsSystem::is_entity_clicked(load_button, mouse_press_x, mouse_press_y);
            if (start_clicked || load_clicked) {
                Mix_PlayChannel(-1, game_start_sound, 0);
                menuState = false;
                load_game = load_clicked;
                restart();
            }
        }
	}
    else if (current_turn < MAX_TURNS)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !blobuleMoved)
        {
            mouse_move = PhysicsSystem::is_entity_clicked(active_player, mouse_press_x, mouse_press_y);
            if (mouse_move){
                Mix_PlayChannel(-1, slingshot_pull_sound, 0);
            }
        }

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE && !blobuleMoved)
        {
            // check if left mouse click was on the asset
            if (mouse_move)
            {
                Mix_PlayChannel(-1, slingshot_shot_sound, 0);
                Mix_PlayChannel(-1, blobule_yipee_sound, 0);

                mouse_move = false;

                auto& blobMotion = ECS::registry<Motion>.get(active_player);
                float blobAngle = blobMotion.angle;
                float blobPower = blobMotion.dragDistance;

                blobMotion.velocity = { cos(blobAngle) * blobPower, sin(blobAngle) * blobPower };

                float velocityMagnitude = Utils::getVelocityMagnitude(blobMotion);
                if (velocityMagnitude > max_blobule_speed) {
                    blobMotion.velocity = { cos(blobAngle) * max_blobule_speed, sin(blobAngle) * max_blobule_speed };
                }

                Blobule::removeTrajectory(active_player);
                blobuleMoved = true;
            }
        }

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            auto save_clicked = PhysicsSystem::is_entity_clicked(save_button, mouse_press_x, mouse_press_y);
            if (save_clicked) {
                //call save function here
                int currPlayer = playerMove;
                if (blobuleMoved) {
                    currPlayer++;
                    if (currPlayer > 3) {
                        currPlayer = 0;
                    }
                }
                MapLoader::saveMap(currPlayer, current_turn);
            }
        }
    }
}
