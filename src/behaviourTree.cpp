#include "behaviourTree.hpp"
#include <functional>
#include <cstdlib>
#include "tiny_ecs.hpp"
#include <iostream>

// A composite node that loops through all children and exits when one fails

BTRepeatingSequence::BTRepeatingSequence(std::vector<std::shared_ptr<BTNode>> children)
{
	m_children = children;
	m_index = 0;
}

void BTRepeatingSequence::init(ECS::Entity e) {
		m_index = 0;
		assert(m_index < m_children.size());
		// initialize the first child
		const auto& child = m_children[m_index];
		assert(child);
		child->init(e);
}

BTState BTRepeatingSequence::process(ECS::Entity e)  {
	if (m_index >= m_children.size())
	{
		BTRepeatingSequence::init(e);
		return BTState::Running;
	}

	// process current child
	const auto& child = m_children[m_index];
	assert(child);
	BTState state = child->process(e);

	// select a new active child and initialize its internal state
	if (state == BTState::Success) {
		++m_index;
		if (m_index >= m_children.size()) {
			init(e);
		}
		else {
			const auto& nextChild = m_children[m_index];
			assert(nextChild);
			nextChild->init(e);
		}
		return BTState::Running;
	}
	else {
		return state;
	}
}

