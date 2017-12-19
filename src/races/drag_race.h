//
// Created by Gijs Sijpesteijn on 18/11/2017.
//

#ifndef CARMAGEDDON_DRAG_RACE_H
#define CARMAGEDDON_DRAG_RACE_H


#include "race.h"
#include <string>
#include <list>
#include <map>
#include <memory>
#include <restbed>

class drag_race: public race {
public:
    drag_race(Camera *camera);
    std::list<observer*> getObservers();
    json_t*  getJson(void);
    void updateWithJson(json_t *json, int start);
    observer* findObserver(std::string type);
    void saveSettings();
private:
    void resetAllObservers();
    settings *sett;
};


#endif //CARMAGEDDON_DRAG_RACE_H
