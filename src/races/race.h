//
// Created by Gijs Sijpesteijn on 18/11/2017.
//

#ifndef CARMAGEDDON_RACE_H
#define CARMAGEDDON_RACE_H

#include "../domain/observer.h"
#include "../util/observer_group.h"
#include "../domain/car.h"
#include <restbed>
#include <jansson.h>
#include <string>

class race {
public:
    race();
    race(std::string name, Car *car, Camera *camera, settings *settings, observer_group *group);

    std::string name;
    Camera *camera;
    Car *car;
    bool running = false;
    observer_group *group = NULL;

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

protected:
    bool selected = false;
    settings *sett;
    pthread_t observer_runner;
};

#endif //CARMAGEDDON_RACE_H
