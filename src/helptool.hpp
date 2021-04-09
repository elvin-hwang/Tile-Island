#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

// help tool
struct HelpTool
{
    static ECS::Entity createHelpTool(vec2 position);
    static void handleHelpToolClicks(double mouse_x, double mouse_y);
};
