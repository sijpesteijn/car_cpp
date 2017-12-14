//
// Created by Gijs Sijpesteijn on 18/11/2017.
//

#ifndef CARMAGEDDON_RACE_H
#define CARMAGEDDON_RACE_H

#include "../domain/observer.h"
#include <restbed>
#include <jansson.h>

class race {
public:
    virtual ~race() {};
    observer *obs;
    virtual std::list<observer*> getObservers() = 0;
    virtual json_t* getJson(void) =0;
    virtual void updateWithJson(json_t *json, int start) =0;
    const char *name;
    int running = 0;
    const char* getName(void) {
        return this->name;
    }
    Camera *camera;

    virtual void saveSettings() = 0;
};

#endif //CARMAGEDDON_RACE_H
