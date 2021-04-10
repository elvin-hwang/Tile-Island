#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"

struct LevelEditor
{
    // Any placeable entity that can be used in the level editor
    enum class EditorEntity {
        YellowBlob,
        GreenBlob,
        RedBlob,
        BlueBlob,
        Egg,
        Water,
        Block,
        Ice,
        Mud,
        Sand,
        Acid,
        Speed,
        Speed_UP,
        Speed_LEFT,
        Speed_RIGHT,
        Speed_DOWN,
        Teleport,
    };

	static void place_entity(std::vector<std::vector<ECS::Entity>>& grid, EditorEntity entity, vec2 grid_coords);
	static void save_map(std::vector<std::vector<ECS::Entity>> grid);
    static void clear_entity_lists();
    static void add_blobule(ECS::Entity blobule);
};
