#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

struct Camera {
	static ECS::Entity createCamera(vec2 position);
};