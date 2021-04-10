// Header
#include "debug.hpp"
#include "tiny_ecs.hpp"
#include "render.hpp"

#include <cmath>
#include <iostream>

#include "render_components.hpp"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 2
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
namespace DebugSystem 
{
	void createLine(vec2 position, vec2 scale, float angle) {
		auto entity = ECS::Entity();

		std::string key = "thick_line";
		ShadedMesh& resource = cache_resource(key);
		if (resource.effect.program.resource == 0) {

			constexpr float z = -0.1f;
			vec3 red = { 0.8,0.1,0.1 };

			// Corner points
			ColoredVertex v;
			v.position = {-0.5,-0.5,z};
			v.color = red;
			resource.mesh.vertices.push_back(v);
			v.position = { -0.5,0.5,z };
			v.color = red;
			resource.mesh.vertices.push_back(v);
			v.position = { 0.5,0.5,z };
			v.color = red;
			resource.mesh.vertices.push_back(v);
			v.position = { 0.5,-0.5,z };
			v.color = red;
			resource.mesh.vertices.push_back(v);

			// Two triangles
			resource.mesh.vertex_indices.push_back(0);
			resource.mesh.vertex_indices.push_back(1);
			resource.mesh.vertex_indices.push_back(3);
			resource.mesh.vertex_indices.push_back(1);
			resource.mesh.vertex_indices.push_back(2);
			resource.mesh.vertex_indices.push_back(3);

			RenderSystem::createColoredMesh(resource, "colored_mesh");
		}

		// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
		ECS::registry<ShadedMeshRef>.emplace(entity, resource);

		// Create motion
		auto& motion = ECS::registry<Motion>.emplace(entity);
		motion.angle = angle;
		motion.velocity = { 0, 0 };
		motion.position = position;
		motion.scale = scale;
		motion.isCollidable = false;

		ECS::registry<DebugComponent>.emplace(entity);
	}

	void clearDebugComponents() {
		// Clear old debugging visualizations
		while (ECS::registry<DebugComponent>.entities.size() > 0) {
			ECS::ContainerInterface::remove_all_components_of(ECS::registry<DebugComponent>.entities.back());
        }
	}

	void createBox(vec2 position, vec2 size, float angle)
	{
		auto scale_horizontal_line = size;
		scale_horizontal_line.y *= 0.08f;
		auto scale_vertical_line = size;
		scale_vertical_line.x *= 0.08f;
		float biggerSize;

		vec2 topPoint = position;
		topPoint.y -= size.y / 2;

		float x = ((topPoint.x - position.x) * cos(angle) - (topPoint.y - position.y) * sin(angle)) + position.x;
		float y = ((topPoint.x - position.x) * sin(angle) + (topPoint.y - position.y) * cos(angle)) + position.y;
		topPoint = { x, y };

		vec2 bottomPoint = position;
		bottomPoint.y += size.y / 2;

		x = ((bottomPoint.x - position.x) * cos(angle) - (bottomPoint.y - position.y) * sin(angle)) + position.x;
		y = ((bottomPoint.x - position.x) * sin(angle) + (bottomPoint.y - position.y) * cos(angle)) + position.y;
		bottomPoint = { x, y };

		vec2 leftPoint = position;
		leftPoint.x -= size.x / 2;

		x = ((leftPoint.x - position.x) * cos(angle) - (leftPoint.y - position.y) * sin(angle)) + position.x;
		y = ((leftPoint.x - position.x) * sin(angle) + (leftPoint.y - position.y) * cos(angle)) + position.y;
		leftPoint = { x, y };

		vec2 rightPoint = position;
		rightPoint.x += size.x / 2;

		x = ((rightPoint.x - position.x) * cos(angle) - (rightPoint.y - position.y) * sin(angle)) + position.x;
		y = ((rightPoint.x - position.x) * sin(angle) + (rightPoint.y - position.y) * cos(angle)) + position.y;
		rightPoint = { x, y };

		DebugSystem::createLine(topPoint, scale_horizontal_line, angle);
		DebugSystem::createLine(bottomPoint, scale_horizontal_line, angle);
		DebugSystem::createLine(leftPoint, scale_vertical_line, angle);
		DebugSystem::createLine(rightPoint, scale_vertical_line, angle);
	}

	void createDirectionLine(vec2 position, vec2 velocity, vec2 size)
	{
		auto scale_horizontal_line = size;
		scale_horizontal_line.y *= 0.2f;
		scale_horizontal_line.x *= 1.2f;

		vec2 topPoint = position;
		topPoint.y -= size.y / 2;
		topPoint.x += size.x / 2;

		vec2 bottomPoint = position;
		bottomPoint.y += size.y / 2;
		bottomPoint.x += size.x / 2;

		vec2 futurePos = position + velocity;

		float y = futurePos.y - position.y;
		float x = futurePos.x - position.x;

		float angle = atan2(y, x);

		DebugSystem::createLine(position, scale_horizontal_line, angle);
	}

	bool in_debug_mode = false;
}
