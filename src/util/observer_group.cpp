//
// Created by Gijs Sijpesteijn on 19/12/2017.
//

#include "observer_group.h"

using namespace std;

observer_group::observer_group(string name, list<observer*> observers) {
    this->name = name;
    this->observers = observers;
}

bool observer_group::isFinished() {
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

void observer_group::setSelected(bool selected) {
    this->selected = selected;
    for(observer *obs: this->observers) {
        obs->setSelected(this->selected);
    }
}

bool observer_group::isSelected() {
    return this->selected;
}

void observer_group::setRunning(bool running) {
    this->running = running;
    for (observer *obs: this->observers) {
        obs->setRunning(running);
    }
//    string time = to_string(chrono::system_clock::now().time_since_epoch().count());
//    for( observer *obs: this->observers) {
//        if (this->running) {
//            obs->setOutputDir(string(this->baseImagePath)
//                                      .append(this->name).append("/")
//                                      .append(time).append("/"));
//        } else {
//            obs->setOutputDir(string(this->baseImagePath).append(this->name).append("/")
//                                      .append(obs->getType()).append("/"));
//        }
//    }
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
}

void observer_group::setOutputDir(std::string outputDir) {
    this->baseImagePath = outputDir;
    for( observer *obs: this->observers) {
        obs->setOutputDir(string(this->baseImagePath).append(this->name).append("/")
                                  .append(obs->getType()).append("/"));
    }
}
