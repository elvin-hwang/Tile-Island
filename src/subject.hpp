
#include "tiny_ecs.hpp"

class Subject
{
    std::list<int> observers_list;
    // Create all the associated render resources and default transform.
    void add_observer(auto lambda);
    void notify();
    //void getState();
};

