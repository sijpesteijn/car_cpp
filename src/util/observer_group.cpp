//
// Created by Gijs Sijpesteijn on 19/12/2017.
//

#include "observer_group.h"

using namespace std;

observer_group::observer_group(list<observer*> observers) {
    this->observers = observers;
}

int observer_group::finished() {
    int finished = 1;
    for (observer *obs: this->observers) {
        if (obs->condition_achieved == 0) {
            finished = 0;
            continue;
        }
    }
    return finished;
}