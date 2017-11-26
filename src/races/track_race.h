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
    std::string getJson(void);
    void updateWithJson(json_t* json);
    std::list<observer*> observers;
};


#endif //CARMAGEDDON_TRACK_RACE_H
