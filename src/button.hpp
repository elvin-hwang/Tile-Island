#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

struct Button
{
	// Create all the associated render resources and default transform.
	static ECS::Entity createButton(vec2 position, vec2 scale, std::string buttonstring);
	
};

