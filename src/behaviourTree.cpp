#include "behaviourTree.hpp"
#include <functional>
#include <cstdlib>
#include "tiny_ecs.hpp"
#include <iostream>

// redefining cuz im not sure how to set lambda function in utils class
auto euclideanDist = [](Motion& fishMotion, Motion& turtleMotion)
{
	float x = fishMotion.position.x - turtleMotion.position.x;
	float y = fishMotion.position.y - turtleMotion.position.y;
	float dist = (float)pow(x, 2) + (float)pow(y, 2);
	return sqrt(dist);
};

// ################################################################################

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

// ################################################################################

BTIfElseCondition::BTIfElseCondition(std::shared_ptr<BTNode> child1, std::shared_ptr<BTNode> child2, std::function<bool(ECS::Entity)> condition)
{
	m_child1 = child1;
	m_child2 = child2;
	m_condition = condition;
}

void BTIfElseCondition::init(ECS::Entity e)
{
	m_index = 0;
	m_child1->init(e);
	m_child2->init(e);
}

BTState BTIfElseCondition::process(ECS::Entity e) {
	// update internal state

	if (m_condition(e))
	{
		//m_child1->init(e);
		return m_child1->process(e);
	}
	else {
		//m_child2->init(e);
		return m_child2->process(e);
	}
}

// ################################################################################

BTAndSequence::BTAndSequence(std::vector<std::shared_ptr<BTNode>> children)
		: m_children(std::move(children)), m_index(0) {
	}

void BTAndSequence::init(ECS::Entity e) 
{
	m_index = 0;
	assert(m_index < m_children.size());
	// initialize the first child
	const auto& child = m_children[m_index];
	assert(child);
	child->init(e);
}

BTState BTAndSequence::process(ECS::Entity e) {
	if (m_index >= m_children.size())
		return BTState::Success;

	// process current child
	const auto& child = m_children[m_index];
	assert(child);
	BTState state = child->process(e);

	// select a new active child and initialize its internal state
	if (state == BTState::Success) {
		++m_index;
		if (m_index >= m_children.size()) {
			return BTState::Success;
		}
		else {
			const auto& nextChild = m_children[m_index];
			assert(nextChild);
			nextChild->init(e);
			return BTState::Running;
		}
	}
	else {
		return state;
	}
}

// ################################################################################

MoveXDirection::MoveXDirection(int steps, float speed)
{
	m_targetSteps = steps; 
	m_stepsRemaining = 0;
	m_speed = speed;
}


void MoveXDirection::init(ECS::Entity e) {
	m_stepsRemaining = m_targetSteps;
}

BTState MoveXDirection::process(ECS::Entity e) {
	// update internal state
	--m_stepsRemaining;

	// modify world
	auto& motion = ECS::registry<Motion>.get(e);
	motion.velocity.y = 0;
	motion.velocity.x = motion.direction.x * m_speed;

	// return progress
	if (m_stepsRemaining > 0) {
		return BTState::Running;
	}
	else {
		return BTState::Success;
	}
}

// ################################################################################

MoveYDirection::MoveYDirection(int steps, float speed)
{
	m_targetSteps = steps;
	m_stepsRemaining = 0;
	m_speed = speed;
}


void MoveYDirection::init(ECS::Entity e) {
	m_stepsRemaining = m_targetSteps;
}

BTState MoveYDirection::process(ECS::Entity e) {
	// update internal state
	--m_stepsRemaining;

	// modify world
	auto& motion = ECS::registry<Motion>.get(e);
	motion.velocity.x = 0;
	motion.velocity.y = motion.direction.y * m_speed;

	// return progress
	if (m_stepsRemaining > 0) {
		return BTState::Running;
	}
	else {
		return BTState::Success;
	}
}

// ################################################################################

void TurnX::init(ECS::Entity e) {

}

BTState TurnX::process(ECS::Entity e) {
	auto& motion = ECS::registry<Motion>.get(e);
	motion.direction.x = -motion.direction.x;

	return BTState::Success;
}

// ################################################################################

void TurnY::init(ECS::Entity e) {

}

BTState TurnY::process(ECS::Entity e) {
	auto& motion = ECS::registry<Motion>.get(e);
	motion.direction.y = -motion.direction.y;

	return BTState::Success;
}

// ################################################################################

void Flee::init(ECS::Entity e) {

}


BTState Flee::process(ECS::Entity e) {
	// modify world
	auto& motion = ECS::registry<Motion>.get(e);
	float angle = 0;
	float closestDist = 1000000.f;

	for (ECS::Entity blob : ECS::registry<Blobule>.entities)
	{
		Motion& blobMotion = ECS::registry<Motion>.get(blob);
		float dist = euclideanDist(blobMotion, motion);
		if (dist < closestDist) {
			closestDist = dist;
			angle = -atan2(blobMotion.position.y - motion.position.y, blobMotion.position.x - motion.position.x);
		}
	}
	motion.velocity = { -cos(angle) * 100.f, sin(angle) * 100.f };
	motion.direction = { motion.velocity.x / abs(motion.velocity.x), motion.velocity.y / abs(motion.velocity.y) };

	// return progress
	return BTState::Success;
}
