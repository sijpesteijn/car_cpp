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
    std::list<observer_group *> getObserverGroups(json_t* root, Camera *camera, Car *car);
    std::list<observer*> getObservers(json_t* root, Camera *camera, Car *car);
};


#endif //CARMAGEDDON_RACE_BUILDER_H
