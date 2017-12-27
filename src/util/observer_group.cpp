//
// Created by Gijs Sijpesteijn on 19/12/2017.
//

#include "observer_group.h"

using namespace std;

observer_group::observer_group(string name, list<observer*> observers) {
    this->name = name;
    this->observers = observers;
}

bool observer_group::finished() {
    int finished = true;
    for (observer *obs: this->observers) {
        if (obs->condition_achieved == false) {
            finished = false;
            continue;
        }
    }
    return finished;
}

void observer_group::processSnapshot(cv::Mat mat) {
    for (observer *obs: this->observers) {
        obs->processSnapshot(mat);
    }
}

void observer_group::setActive(bool active) {
    if (this->active != active) {
        this->active = active;
        for(observer *obs: this->observers) {
            obs->setActive(this->active);
        }
        if (this->next) {
            this->next->setActive(active);
        }
    }
}

observer *observer_group::getObserver(string type) {
    for (observer *obs: this->observers) {
        if (obs->getType().compare(type) == 0) {
            return obs;
        }
    }
    return NULL;
}

void observer_group::reset() {
    for( observer *obs: this->observers) {
        obs->condition_achieved = false;
    }
    if (this->next) {
        this->next->reset();
    }
}

void observer_group::setOutputDir(std::string outputDir) {
    for( observer *obs: this->observers) {
        obs->setOutputDir(std::string(outputDir).append(this->name).append("/"));
    }
    if (this->next) {
        this->next->setOutputDir(outputDir);
    }
}
