#pragma once

// stlib
#include <string>
#include <tuple>
#include <vector>
#include <stdexcept>

// glfw (OpenGL)
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

// The glm library provides vector and matrix operations as in GLSL
#include <glm/vec2.hpp>				// vec2
#include <glm/ext/vector_int2.hpp>  // ivec2
#include <glm/vec3.hpp>             // vec3
#include <glm/mat3x3.hpp>           // mat3
using namespace glm;
static const float PI = 3.14159265359f;

static const float tileSize = 45.f;

// Simple utility functions to avoid mistyping directory name
inline std::string data_path() { return "data"; };
inline std::string shader_path(const std::string& name) { return data_path() + "/shaders/" + name;};
inline std::string textures_path(const std::string& name) { return data_path() + "/textures/" + name; };
inline std::string audio_path(const std::string& name) { return data_path() + "/audio/" + name; };
inline std::string mesh_path(const std::string& name) { return data_path() + "/meshes/" + name; };

// The 'Transform' component handles transformations passed to the Vertex shader
// (similar to the gl Immediate mode equivalent, e.g., glTranslate()...)
struct Transform {
	mat3 mat = { { 1.f, 0.f, 0.f }, { 0.f, 1.f, 0.f}, { 0.f, 0.f, 1.f} }; // start with the identity
	void scale(vec2 scale);
	void rotate(float radians);
	void translate(vec2 offset);
};

// All data relevant to the shape and motion of entities
struct Motion {
	vec2 position = { 0, 0 };
	float angle = 0;
	float friction = 0;
	float dragDistance = 0;
	vec2 velocity = { 0, 0 };
	vec2 direction = { 0, 0 };
	vec2 scale = { 0, 0 };
	bool isCollidable = false;
	std::string shape = "square";
};

// active player shared as global variable

enum class EggState { normal, move };

struct EggAi {
	EggState state = EggState::normal;
	bool initBehaviour = false;
};

// Components to keep track of each color's splat count
struct RedSplat {};
struct BlueSplat {};
struct GreenSplat {};
struct YellowSplat {};

// Keep track of game state and different screens
enum class GameState {
	Start,
	Intro,
	Yellow,
	Green,
	Red,
	Blue,
	Paint,
	Island,
	Tutorial,
	Level,
	Game,
	LevelEditor,
};

static const float tile_width = 44.46f;
