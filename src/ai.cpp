// internal
#include "ai.hpp"
#include "tiny_ecs.hpp"
#include "blobule.hpp"
#include "utils.hpp"

#include <iostream>
#include <cstdlib>
#include <random>
#include <functional>

float maxDistanceFromEgg = 150.f;

float eggSpeed = 75.f;


auto euclideanDist = [](Motion& fishMotion, Motion& turtleMotion)
{
	float x = fishMotion.position.x - turtleMotion.position.x;
	float y = fishMotion.position.y - turtleMotion.position.y;

	float dist = (float)pow(x, 2) + (float)pow(y, 2);
	return sqrt(dist);
};

auto checkNearbyBlobules = [](ECS::Entity e) {
	for (ECS::Entity& blob : ECS::registry<Blobule>.entities)
	{
		Motion& eggMotion = ECS::registry<Motion>.get(e);
		Motion& blobMotion = ECS::registry<Motion>.get(blob);
		if (euclideanDist(eggMotion, blobMotion) <= maxDistanceFromEgg)
			return true;
	}
	return false;
};

// A composite node that loops through all children and exits when one fails
class BTRepeatingSequence : public BTNode {
public:
	BTRepeatingSequence(std::vector<std::shared_ptr<BTNode>> children)
		: m_children(std::move(children)), m_index(0) {
	}

private:
	void init(ECS::Entity e) override
	{
		m_index = 0;
		assert(m_index < m_children.size());
		// initialize the first child
		const auto& child = m_children[m_index];
		assert(child);
		child->init(e);
	}

	BTState process(ECS::Entity e) override {
		if (m_index >= m_children.size())
		{
			init(e);
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

	int m_index;
	std::vector<std::shared_ptr<BTNode>> m_children;
};

// will perform one of the 2 child nodes depending on the result of lambda condition fn
class BTIfElseCondition : public BTNode {
public:
	BTIfElseCondition(std::shared_ptr<BTNode> child1, std::shared_ptr<BTNode> child2, std::function<bool(ECS::Entity)> condition) noexcept
		:  m_child1(child1), m_child2(child2), m_index(0), m_condition(condition) {
	}

private:
	void init(ECS::Entity e) override {
		m_index = 0;
		m_child1->init(e);
		m_child2->init(e);
	}

	BTState process(ECS::Entity e) override {
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

	int m_index;
	std::function<bool(ECS::Entity)> m_condition;
	std::shared_ptr<BTNode> m_child1;
	std::shared_ptr<BTNode> m_child2;
};

// completes all child nodes in order
class BTAndSequence : public BTNode {
public:
	BTAndSequence(std::vector<std::shared_ptr<BTNode>> children)
		: m_children(std::move(children)), m_index(0) {
	}

private:
	void init(ECS::Entity e) override
	{
		m_index = 0;
		assert(m_index < m_children.size());
		// initialize the first child
		const auto& child = m_children[m_index];
		assert(child);
		child->init(e);
	}

	BTState process(ECS::Entity e) override {
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
				std::cout << "next state" << std::endl;
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

	int m_index;
	std::vector<std::shared_ptr<BTNode>> m_children;
};

class MoveXDirection : public BTNode {
public:
	MoveXDirection(int steps) noexcept
		: m_targetSteps(steps), m_stepsRemaining(0) {
	}

private:
	void init(ECS::Entity e) override {
		m_stepsRemaining = m_targetSteps;
	}

	BTState process(ECS::Entity e) override {
		// update internal state
		--m_stepsRemaining;

		// modify world
		auto& motion = ECS::registry<Motion>.get(e);
		motion.velocity.y = 0;
		motion.velocity.x = motion.direction.x * eggSpeed;

		// return progress
		if (m_stepsRemaining > 0) {
			return BTState::Running;
		}
		else {
			return BTState::Success;
		}
	}

private:
	int m_targetSteps;
	int m_stepsRemaining;
};

class MoveYDirection : public BTNode {
public:
	MoveYDirection(int steps) noexcept
		: m_targetSteps(steps), m_stepsRemaining(0) {
	}

private:
	void init(ECS::Entity e) override {
		m_stepsRemaining = m_targetSteps;
	}

	BTState process(ECS::Entity e) override {
		// update internal state
		--m_stepsRemaining;

		// modify world
		auto& motion = ECS::registry<Motion>.get(e);
		motion.velocity.x = 0;
		motion.velocity.y = motion.direction.y * eggSpeed;

		// return progress
		if (m_stepsRemaining > 0) {
			return BTState::Running;
		}
		else {
			return BTState::Success;
		}
	}

private:
	int m_targetSteps;
	int m_stepsRemaining;
};

class TurnX : public BTNode {
private:
	void init(ECS::Entity e) override {

	}

	BTState process(ECS::Entity e) override {
		auto& motion = ECS::registry<Motion>.get(e);
		motion.direction.x = -motion.direction.x;
		std::cout << "turning " << std::endl;

		return BTState::Success;
	}

};

class TurnY : public BTNode {
private:
	void init(ECS::Entity e) override {

	}

	BTState process(ECS::Entity e) override {
		auto& motion = ECS::registry<Motion>.get(e);
		motion.direction.y = -motion.direction.y;

		return BTState::Success;
	}

};


class Flee : public BTNode {
private:
	void init(ECS::Entity e) override {
	}

	BTState process(ECS::Entity e) override {
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
};

std::shared_ptr <BTNode> moveX = std::make_unique<MoveXDirection>(20);
std::shared_ptr <BTNode> moveY = std::make_unique<MoveYDirection>(20);
std::shared_ptr <BTNode> turnX = std::make_unique<TurnX>();
std::shared_ptr <BTNode> turnY = std::make_unique<TurnY>();
std::shared_ptr <BTNode> flee = std::make_unique<Flee>();
std::shared_ptr <BTNode> moveSquare = std::make_unique<BTAndSequence>(std::vector<std::shared_ptr <BTNode>>({ moveX, turnX, moveY, turnY }));
std::shared_ptr <BTNode> moveOrFlee = std::make_unique<BTIfElseCondition>(flee, moveSquare, checkNearbyBlobules);

AISystem::AISystem()
{
	// initializing 
	root_run_and_return = std::make_unique<BTRepeatingSequence>(std::vector<std::shared_ptr <BTNode>>({ moveOrFlee }));
	std::cout << ECS::registry<EggAi>.entities.size() << std::endl;
}

void AISystem::step(float elapsed_ms, vec2 window_size_in_game_units)
{
	(void)elapsed_ms; // placeholder to silence unused warning until implemented
	(void)window_size_in_game_units; // placeholder to silence unused warning until implemented

	for (ECS::Entity& eggNPC : ECS::registry<EggAi>.entities)
	{
		EggAi& eggAi = ECS::registry<EggAi>.get(eggNPC);
		if (!eggAi.initBehaviour)
		{
			root_run_and_return->init(eggNPC);
			eggAi.initBehaviour = true;
		}
		root_run_and_return->process(eggNPC);
	}

	// add other ai steps...
}
