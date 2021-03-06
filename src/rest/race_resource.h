//
// Created by Gijs Sijpesteijn on 18/11/2017.
//

#ifndef CARMAGEDDON_RACE_RESOURCE_H
#define CARMAGEDDON_RACE_RESOURCE_H


#include "abstract_rest_resource.h"
#include "../races/race.h"
#include "../domain/car.h"
#include <list>
#include <map>
#include <restbed>
#include <string>

class race_resource: public abstract_rest_resource {
public:
    race_resource(Camera *camera, Car *car);
    std::list<std::shared_ptr<restbed::Resource>> getResources();
    std::map<std::string, race*> races;
    race* getRace(std::string name);
    race *selected_race = NULL;
    Car *car = NULL;
    pthread_t race_status_runner;
private:
    std::shared_ptr<restbed::Resource> allRaceResource = std::make_shared< restbed::Resource >( );
    std::shared_ptr<restbed::Resource> singleRaceResource = std::make_shared< restbed::Resource >( );
    std::shared_ptr<restbed::Resource> selectRaceResource = std::make_shared< restbed::Resource >( );
    std::shared_ptr<restbed::Resource> statusRaceResource = std::make_shared< restbed::Resource >( );
    std::shared_ptr<restbed::Resource> observerPreviewResource = std::make_shared< restbed::Resource >( );
};


#endif //CARMAGEDDON_RACE_RESOURCE_H
