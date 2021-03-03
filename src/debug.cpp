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
	void createLine(vec2 position, vec2 scale) {
		auto entity = ECS::Entity();

		std::string key = "thick_line";
		ShadedMesh& resource = cache_resource(key);
		if (resource.effect.program.resource == 0) {
			// create a procedural circle
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
		motion.angle = 0.f;
		motion.velocity = { 0, 0 };
		motion.position = position;
		motion.scale = scale;

		ECS::registry<DebugComponent>.emplace(entity);
	}

	void clearDebugComponents() {
		// Clear old debugging visualizations
		while (ECS::registry<DebugComponent>.entities.size() > 0) {
			ECS::ContainerInterface::remove_all_components_of(ECS::registry<DebugComponent>.entities.back());
        }
	}

	void createBox(vec2 position, vec2 size)
	{
		auto scale_horizontal_line = size;
		scale_horizontal_line.y *= 0.05f;
		auto scale_vertical_line = size;
		scale_vertical_line.x *= 0.05f;

		vec2 topPoint = position;
		topPoint.y -= size.y / 2;

		vec2 bottomPoint = position;
		bottomPoint.y += size.y / 2;

		vec2 leftPoint = position;
		leftPoint.x -= size.x / 2;

		vec2 rightPoint = position;
		rightPoint.x += size.x / 2;

		DebugSystem::createLine(topPoint, scale_horizontal_line);
		DebugSystem::createLine(bottomPoint, scale_horizontal_line);
		DebugSystem::createLine(leftPoint, scale_vertical_line);
		DebugSystem::createLine(rightPoint, scale_vertical_line);
	}

	bool in_debug_mode = false;
}
