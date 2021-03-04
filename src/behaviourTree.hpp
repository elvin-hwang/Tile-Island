#pragma once


#include <vector>

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "world.hpp"
#include <iostream>
#include <functional>

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
};

// Composite node that loops through all children and exits when one fails
class BTRepeatingSequence : public BTNode {

public:
	BTRepeatingSequence(std::vector<std::shared_ptr<BTNode>> children);
	void init(ECS::Entity e) override;

	BTState process(ECS::Entity e) override;

	int m_index;
	std::vector<std::shared_ptr<BTNode>> m_children;
};

// will perform one of the 2 child nodes depending on the result of lambda condition fn
class BTIfElseCondition : public BTNode {
public:
	BTIfElseCondition(std::shared_ptr<BTNode> child1, std::shared_ptr<BTNode> child2, std::function<bool(ECS::Entity)> condition);

private:
	void init(ECS::Entity e) override;

	BTState process(ECS::Entity e) override;

	int m_index;
	std::function<bool(ECS::Entity)> m_condition;
	std::shared_ptr<BTNode> m_child1;
	std::shared_ptr<BTNode> m_child2;
};

class BTAndSequence : public BTNode {
public:
	BTAndSequence(std::vector<std::shared_ptr<BTNode>> children);

private:
	void init(ECS::Entity e) override;

	BTState process(ECS::Entity e) override;

	int m_index;
	std::vector<std::shared_ptr<BTNode>> m_children;
};

class MoveXDirection : public BTNode {
public:
	MoveXDirection(int steps, float speed);

private:
	void init(ECS::Entity e) override;

	BTState process(ECS::Entity e) override;

	int m_targetSteps;
	int m_stepsRemaining;
	float m_speed;
};

class MoveYDirection : public BTNode {
public:
	MoveYDirection(int steps, float speed);

private:
	void init(ECS::Entity e) override;

	BTState process(ECS::Entity e) override;

	int m_targetSteps;
	int m_stepsRemaining;
	float m_speed;
};

class TurnX : public BTNode {

private:
	void init(ECS::Entity e) override;
	BTState process(ECS::Entity e) override;
};

class TurnY : public BTNode {

private:
	void init(ECS::Entity e) override;
	BTState process(ECS::Entity e) override;
};

class Flee : public BTNode {

private:
	void init(ECS::Entity e) override;
	BTState process(ECS::Entity e) override;
};
