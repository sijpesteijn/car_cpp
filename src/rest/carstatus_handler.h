//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_CARSTATUS_HANDLER_H
#define CARMAGEDDON_CARSTATUS_HANDLER_H


#include <restbed>
#include "../domain/car.h"

class carstatus_handler {
public:
    carstatus_handler(Car *car);
    std::shared_ptr<restbed::Resource> getResource();
private:
    std::shared_ptr<restbed::Resource> resource;
};

#endif //CARMAGEDDON_CARSTATUS_HANDLER_H
