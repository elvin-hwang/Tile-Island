#include "tiny_ecs.hpp"
#include <functional>

class Subject
{
public:
    std::list<std::function<void(ECS::Entity, ECS::Entity)>> observers_list;
    ECS::Entity createSubject(std::string name);
    void add_observer(const std::function<void(ECS::Entity, ECS::Entity)>& lambda);
    void notify(ECS::Entity entity, ECS::Entity entity_other);
};
