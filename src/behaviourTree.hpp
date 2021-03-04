#pragma once


#include <vector>

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "world.hpp"
#include <iostream>

// The return type of behaviour tree processing
enum class BTState {
	Running,
	Success,
	Failure
};

// The base class representing any node in our behaviour tree
class BTNode {
public:
	virtual ~BTNode() noexcept = default; // Needed for deletion-through-pointer

	virtual void init(ECS::Entity e) {};

	virtual BTState process(ECS::Entity e) = 0;

	int m_index;
	std::vector<std::shared_ptr<BTNode>> m_children;
};

class BTRepeatingSequence : public BTNode {

public:
	BTRepeatingSequence(std::vector<std::shared_ptr<BTNode>> children);
	void init(ECS::Entity e) override;

	BTState process(ECS::Entity e) override;
};