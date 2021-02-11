#include "subject.hpp"

void Subject::add_observer(auto lambda)
{
	observers_list.push_back(lambda);
}

void Subject::notify() {
    for (int i = 0; i < observers_list.size(); i++)
		observers_list[i]->update();
}
