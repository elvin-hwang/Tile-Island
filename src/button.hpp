#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

enum ButtonEnum {
	SaveGame,
	LoadGame,
	LoadGame_Settings,
	OpenSettings,
	StartGame,
	ExitSettings,
	LoadMaps,
	RestartGame,
	QuitGame
};

struct Button
{
	// Create all the associated render resources and default transform.
	static ECS::Entity createButton(vec2 position, vec2 scale, ButtonEnum buttonEnum, std::string buttonstring);
	ECS::Entity text_entity;
};

