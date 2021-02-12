#include "subject.hpp"
#include <functional>

ECS::Entity Subject::createSubject(std::string name)
{
    auto entity = ECS::Entity();
    std::string key = name;
    auto& subj = ECS::registry<Subject>.emplace(entity);
    return entity;
}

void Subject::notify(ECS::Entity entity, ECS::Entity entity_other) {
    for (auto o : observers_list) {
        //updating all the observers, calling the lambda function
        o(entity, entity_other);
    }
}
