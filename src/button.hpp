#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

enum class buttonType {
	Start,
	Load
};


struct Button
{
	buttonType buttonEnum;

	// Create all the associated render resources and default transform.
	static ECS::Entity createButton(vec2 position, vec2 scale, buttonType type, std::string buttonstring);
	
};

