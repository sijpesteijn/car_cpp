//
// Created by Gijs Sijpesteijn on 22/12/2017.
//

#ifndef CARMAGEDDON_RACE_BUILDER_H
#define CARMAGEDDON_RACE_BUILDER_H

#include <string>
#include "../races/race.h"

class race_builder {

public:
    race_builder();
    std::map<std::string, race*> getRaces(std::string path, Car *car, Camera* camera);

private:
    observer_group *getObserverGroup(json_t* root, Camera *camera);
    std::list<observer*> getObservers(json_t* root, Camera *camera);
};


#endif //CARMAGEDDON_RACE_BUILDER_H
