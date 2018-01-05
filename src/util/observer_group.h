//
// Created by Gijs Sijpesteijn on 19/12/2017.
//

#ifndef CARMAGEDDON_OBSERVER_GROUP_H
#define CARMAGEDDON_OBSERVER_GROUP_H

#include "../observers/observer.h"
#include <list>
#include <string>

class observer_group {
public:
    observer_group(std::string name, std::list<observer*> observers);
    bool isFinished();
    bool isRunning();
    void processSnapshot(cv::Mat mat);
    std::list<observer*> observers;
    void setSelected(bool selected);
    bool isSelected();
    void setRunning(bool selected);
    observer *getObserver(std::string basic_string);
    void setOutputDir(std::string outputDir);
    std::string name;
private:
    bool selected;
    bool running;
    std::string baseImagePath;
};


#endif //CARMAGEDDON_OBSERVER_GROUP_H
