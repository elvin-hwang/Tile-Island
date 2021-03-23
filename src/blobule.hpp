#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

enum class blobuleCol {
	Red,
	Blue,
	Green,
	Yellow
};

// Blobule characters in game
struct Blobule
{
	vec2 origin;
	std::vector<int> currentGrid;
	std::string color;
	blobuleCol colEnum;
	bool active_player = false;

	ECS::Entity trajectoryEntity;
	// Create all the associated render resources and default transform.
	static ECS::Entity createBlobule(vec2 position, blobuleCol col, std::string colString);

	static void setTrajectory(ECS::Entity entity);

	static void removeTrajectory(ECS::Entity entity);

private:
	ECS::Entity createTrajectory(Blobule& blob);

};

