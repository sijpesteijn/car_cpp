//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_CAR_RESOURCE_H
#define CARMAGEDDON_CAR_RESOURCE_H

#include "../domain/car.h"
#include <restbed>
#include <list>
#include "carmageddon_resource.h"

class car_resource: public carmageddon_resource {
public:
    car_resource(Car *car);
    std::list<std::shared_ptr<restbed::Resource>> getResources();
private:
    std::shared_ptr<restbed::Resource> carGetModeResource = std::make_shared< restbed::Resource >( );
    std::shared_ptr<restbed::Resource> carPostModeResource = std::make_shared< restbed::Resource >( );
    std::shared_ptr<restbed::Resource> steerResource = std::make_shared< restbed::Resource >( );
    std::shared_ptr<restbed::Resource> engineResource = std::make_shared< restbed::Resource >( );
    std::shared_ptr<restbed::Settings> settings = std::make_shared< restbed::Settings >( );
};

#endif //CARMAGEDDON_CAR_RESOURCE_H
