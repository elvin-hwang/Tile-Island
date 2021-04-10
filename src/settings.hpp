#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

struct Settings
{
	// Create all the associated render resources and default transform.
	static ECS::Entity createSettings(vec2 position, vec2 scale);
	static void handleSettingClicks(double mouse_x, double mouse_y);
};

