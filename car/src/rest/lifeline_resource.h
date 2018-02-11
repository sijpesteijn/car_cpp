//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_LIFELINE_RESOURCE_H
#define CARMAGEDDON_LIFELINE_RESOURCE_H

#include <restbed>
#include "../domain/car.h"
#include "abstract_rest_resource.h"

class lifeline_resource: public abstract_rest_resource {
public:
    lifeline_resource(Car *car);
    list<std::shared_ptr<restbed::Resource>> getResources();
private:
    std::shared_ptr<restbed::Resource> resource;
};


#endif //CARMAGEDDON_LIFELINE_RESOURCE_H
