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

float responseDelay = 500.f;
float timer = 0.f;
float angle = 0.f;
int randomScope = 90.f; // The scope of possible random angles (in degrees)

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

class BTIfElseCondition : public BTNode {
public:
	BTIfElseCondition(std::shared_ptr<BTNode> child1, std::shared_ptr<BTNode> child2, int steps, std::function<bool(ECS::Entity)> condition) noexcept
		: m_targetSteps(steps), m_stepsRemaining(0), m_child1(child1), m_child2(child2), m_index(0), m_condition(condition) {
	}

private:
	void init(ECS::Entity e) override {
		m_stepsRemaining = m_targetSteps;
		m_index = 0;
		m_child1->init(e);
		m_child2->init(e);
	}

	BTState process(ECS::Entity e) override {
		// update internal state
		--m_stepsRemaining;

		if (m_condition(e))
		{
			//std::cout << "IF case true" << std::endl;
			return m_child1->process(e);
		}
		else {
			//std::cout << "ELSE case true" << std::endl;
			return m_child2->process(e);
		}
	}

	int m_targetSteps;
	int m_stepsRemaining;
	int m_index;
	std::function<bool(ECS::Entity)> m_condition;
	std::shared_ptr<BTNode> m_child1;
	std::shared_ptr<BTNode> m_child2;
};




// Leaf node examples
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
		motion.position.x += motion.velocity.x;

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

// Leaf node examples
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
		motion.position.y += motion.velocity.y;

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


class TurnAround : public BTNode {
private:
	void init(ECS::Entity e) override {
	}

	BTState process(ECS::Entity e) override {
		// modify world
		auto& vel = ECS::registry<Motion>.get(e).velocity;
		vel = -vel;
		std::cout << "turning around" << std::endl;

		// return progress
		return BTState::Success;
	}
};

std::shared_ptr <BTNode> moveX = std::make_unique<MoveXDirection>(100);
std::shared_ptr <BTNode> moveY = std::make_unique<MoveYDirection>(100);
std::shared_ptr <BTNode> turnX = std::make_unique<TurnAround>();

auto euclideanDist = [](Motion& fishMotion, Motion& turtleMotion)
{
	float x = fishMotion.position.x - turtleMotion.position.x;
	float y = fishMotion.position.y - turtleMotion.position.y;

	float dist = (float)pow(x, 2) + (float)pow(y, 2);
	return sqrt(dist);
};

// TODO: make this euclidean dist check
auto checkNearbyBlobules = [](ECS::Entity e) {
	for (ECS::Entity& blob : ECS::registry<Blobule>.entities)
	{
		Motion& eggMotion = ECS::registry<Motion>.get(e);
		Motion& blobMotion = ECS::registry<Motion>.get(blob);
		if (euclideanDist(eggMotion, blobMotion) <= 200.f)
			return true;
	}
	return false;
};

std::shared_ptr <BTNode> runOrFlee = std::make_unique<BTIfElseCondition>(moveX, moveY, 10, checkNearbyBlobules);

AISystem::AISystem()
{
	// initializing 
	root_run_and_return = std::make_unique<BTRepeatingSequence>(std::vector<std::shared_ptr <BTNode>>({ runOrFlee, turnX, runOrFlee, turnX }));
}

void AISystem::step(float elapsed_ms, vec2 window_size_in_game_units)
{
	(void)elapsed_ms; // placeholder to silence unused warning until implemented
	(void)window_size_in_game_units; // placeholder to silence unused warning until implemented

	// egg ai here
	/*updateEggAiState();
	EggAiActOnState();*/
	timer -= elapsed_ms;

	for (ECS::Entity& eggNPC : ECS::registry<EggAi>.entities)
	{
		EggAi& eggAi = ECS::registry<EggAi>.get(eggNPC);
		if (!eggAi.initBehaviour)
		{
			root_run_and_return->init(eggNPC);
			eggAi.initBehaviour = true;
		}
		auto state = root_run_and_return->process(eggNPC);
		if (state == BTState::Running)
		{
			/*	std::cout << "running!!" << std::endl;
				std::cout << "  velocity = " << ECS::registry<Motion>.get(eggNPC).velocity.x << '\n';
				std::cout << "  position = " << ECS::registry<Motion>.get(eggNPC).position.x << '\n';

				std::cout << "  state = " << static_cast<std::underlying_type<BTState>::type>(state) << '\n';*/
		}
		/*else
		{
			std::cout << "done!!" << std::endl;
			std::cout << "  velocity = " << ECS::registry<Motion>.get(eggNPC).velocity.x << '\n';
			std::cout << "  position = " << ECS::registry<Motion>.get(eggNPC).position.x << '\n';
		}*/
	}


	// add other ai steps...
}

/*
* EggAi performs action based on whatever state is it set to.
*/
void AISystem::EggAiActOnState()
{
	for (ECS::Entity& eggNPC : ECS::registry<EggAi>.entities)
	{
		Motion& eggMotion = ECS::registry<Motion>.get(eggNPC);
		EggAi& eggAi = ECS::registry<EggAi>.get(eggNPC);

		if (eggAi.state == EggState::normal)
		{
			eggMotion.velocity = { 0, 0 };
			timer = 0.f;
		}
		else if (eggAi.state == EggState::move)
		{
			if (timer <= 0.f) {
				angle = (rand() % randomScope + angle - randomScope / 2) * PI / 180;
				timer = responseDelay;
				eggMotion.velocity = { cos(angle) * eggSpeed, sin(angle) * eggSpeed };
			}
		}
	}
}

/*
Updates EggAi State based on various conditions.
All EggAi's will gain THE SAME randomized state for the duration of a players turn which lasts until the end of the players turn.
Upon the next player's turn, the EggAi will get a new randomized state, which lasts for the duration of the active player's turn.
The EggAi will trigger that state whenever the active player's blobule moves within range of the EggAi's entity, and will return back to normal state when blobule moves out of range.
The EggAi ignores all other blobule that does not belong to the active player.
*/
void AISystem::updateEggAiState()
{
	// egg ai state updated here
	for (ECS::Entity& eggNPC : ECS::registry<EggAi>.entities)
	{
		Motion& eggMotion = ECS::registry<Motion>.get(eggNPC);
		EggAi& eggAi = ECS::registry<EggAi>.get(eggNPC);

		ECS::Entity& active_blobule = Utils::getActivePlayerBlobule();

		Motion& blobMotion = ECS::registry<Motion>.get(active_blobule);
		float dist = Utils::euclideanDist(eggMotion, blobMotion);

		if (dist < maxDistanceFromEgg)
		{
			angle = atan2(blobMotion.position.y - eggMotion.position.y, blobMotion.position.x - eggMotion.position.x) * 180 / PI + 180;
			std::string currentActivePlayer = ECS::registry<Blobule>.get(active_blobule).color;
			if (currentActivePlayer != lastActivePlayer)
			{
				lastActivePlayer = currentActivePlayer;
			}
			eggAi.state = EggState::move;
		}
		else {
			eggAi.state = EggState::normal;
		}
	}
}
