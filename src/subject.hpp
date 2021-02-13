#include "tiny_ecs.hpp"
#include <functional>
#include <list>
#include <string>

struct Subject
{
public:
    std::list<std::function<void(ECS::Entity, ECS::Entity)>> observers_list;
    static ECS::Entity createSubject(std::string name);
    template<typename F>
    void add_observer(F lambda)
    {
        observers_list.push_back(lambda);
    }

    void notify(ECS::Entity entity, ECS::Entity entity_other);
};


