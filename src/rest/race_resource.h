//
// Created by Gijs Sijpesteijn on 18/11/2017.
//

#ifndef CARMAGEDDON_RACE_RESOURCE_H
#define CARMAGEDDON_RACE_RESOURCE_H


#include "carmageddon_resource.h"
#include "../races/race.h"
#include <list>
#include <map>
#include <restbed>
#include <string>

class race_resource: public carmageddon_resource {
public:
    race_resource(Camera *camera);
    std::list<std::shared_ptr<restbed::Resource>> getResources();
    std::map<std::string, race*> races;
    race* getRace(std::string name);
private:
    std::shared_ptr<restbed::Resource> allRaceResource = std::make_shared< restbed::Resource >( );
    std::shared_ptr<restbed::Resource> singleRaceResource = std::make_shared< restbed::Resource >( );
    std::shared_ptr<restbed::Resource> statusRaceResource = std::make_shared< restbed::Resource >( );
};


#endif //CARMAGEDDON_RACE_RESOURCE_H
