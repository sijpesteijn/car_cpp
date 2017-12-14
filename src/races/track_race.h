//
// Created by Gijs Sijpesteijn on 18/11/2017.
//

#ifndef CARMAGEDDON_TRACK_RACE_H
#define CARMAGEDDON_TRACK_RACE_H


#include "race.h"
#include <list>


class track_race: public race {
public:
    track_race();
    std::list<observer*> getObservers();
    json_t* getJson(void);
    void updateWithJson(json_t* json, int start);
    std::list<observer*> observers;
    void saveSettings();

private:
    settings *sett;
};


#endif //CARMAGEDDON_TRACK_RACE_H
