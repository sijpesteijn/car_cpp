//
// Created by Gijs Sijpesteijn on 19/12/2017.
//

#ifndef CARMAGEDDON_OBSERVER_GROUP_H
#define CARMAGEDDON_OBSERVER_GROUP_H

#include "../domain/observer.h"
#include <list>

class observer_group {
public:
    observer_group(std::string name, std::list<observer*> observers);
    bool finished();
    observer_group *next;
    void processSnapshot(cv::Mat mat);
    std::list<observer*> observers;
    bool getActive() {
        return this->active;
    }
    void setActive(bool active);
    observer *getObserver(std::string basic_string);
    void reset();
    void setOutputDir(std::string outputDir);
    std::string name;
private:
    bool active;
};


#endif //CARMAGEDDON_OBSERVER_GROUP_H
