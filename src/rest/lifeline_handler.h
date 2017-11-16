//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_LIFELINE_HANDLER_H
#define CARMAGEDDON_LIFELINE_HANDLER_H

#include <restbed>
#include "../domain/car.h"
#include "carmageddon_resource.h"

class lifeline_handler: public carmageddon_resource {
public:
    lifeline_handler(Car *car);
    list<std::shared_ptr<restbed::Resource>> getResources();
private:
    std::shared_ptr<restbed::Resource> resource;
};


#endif //CARMAGEDDON_LIFELINE_HANDLER_H
