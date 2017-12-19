//
// Created by Gijs Sijpesteijn on 19/12/2017.
//

#ifndef CARMAGEDDON_OBSERVER_GROUP_H
#define CARMAGEDDON_OBSERVER_GROUP_H

#include "../domain/observer.h"
#include <list>

class observer_group {
public:
    observer_group(std::list<observer*> observers);
    int active;
    int finished();
    observer_group *next;

private:
    std::list<observer*> observers;
};


#endif //CARMAGEDDON_OBSERVER_GROUP_H
