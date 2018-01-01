//
// Created by Gijs Sijpesteijn on 18/11/2017.
//

#ifndef CARMAGEDDON_RACE_H
#define CARMAGEDDON_RACE_H

#include "../observers/observer.h"
#include "../util/observer_group.h"
#include "../domain/car.h"
#include <restbed>
#include <jansson.h>
#include <string>

class race {
public:
    race();
    race(std::string name, Car *car, Camera *camera, settings *settings, list<observer_group *> groups);

    std::string name;
    Camera *camera;
    Car *car;
    list<observer_group*> groups;

    json_t* getJson(bool full = false);
    void updateWithJson(json_t *json, bool start = false);


    bool getSelected() {
        return this->selected;
    }
    std::string getName(void) {
        return this->name;
    }
    void setSelected(bool selected);
    void saveSettings();

    void setRunning(bool running);

    bool isRunning();

    string getPreviewImageLocation(const string group, const string observer, const string observer_stage);

protected:
    bool running = false;
    bool selected = false;
    settings *sett;
    pthread_t observer_runner;

    observer_group *getObserverGroup(string name);
};

#endif //CARMAGEDDON_RACE_H
