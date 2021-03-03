// internal
#include "ai.hpp"
#include "tiny_ecs.hpp"
#include "blobule.hpp"
#include "utils.hpp"

#include <iostream>
#include <cstdlib>
#include <random>

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

// Leaf node examples
class RunNSteps : public BTNode {
public:
	RunNSteps(int steps) noexcept
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

		std::cout << "moving" << std::endl;

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
		auto& vel = ECS::registry<Motion>.get(e).velocity.x;
		vel = -vel;
		std::cout << "turning around" << std::endl;

		// return progress
		return BTState::Success;
	}
};

std::shared_ptr <BTNode> run3 = std::make_unique<RunNSteps>(10);
std::shared_ptr <BTNode> turn = std::make_unique<TurnAround>();
std::shared_ptr <BTNode> run1 = std::make_unique<RunNSteps>(10);


AISystem::AISystem()
{
	// initializing 
	root_run_and_return = std::make_unique<BTRepeatingSequence>(std::vector<std::shared_ptr <BTNode>>({ run3, turn, run1, turn }));
	std::cout << "init ai" << std::endl;
	std::cout << ECS::registry<EggAi>.entities.size() << std::endl;
}

void AISystem::step(float elapsed_ms, vec2 window_size_in_game_units)
{
	(void)elapsed_ms; // placeholder to silence unused warning until implemented
	(void)window_size_in_game_units; // placeholder to silence unused warning until implemented

	// egg ai here
	/*updateEggAiState();
	EggAiActOnState();*/
	timer -= elapsed_ms;
	//std::cout << "ai thinking" << std::endl;

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
