//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_OBSERVER_RESOURCE_H
#define CARMAGEDDON_OBSERVER_RESOURCE_H

#include <restbed>
#include <list>
#include "carmageddon_resource.h"
#include "../cpu.h"

class observer_handler: public carmageddon_resource {
public:
    observer_handler(cpu *carmageddon);
    list<shared_ptr<restbed::Resource>> getResources();
    cpu *carmageddon;
private:
    shared_ptr<restbed::Resource> observerPostResource = make_shared< restbed::Resource >( );
    shared_ptr<restbed::Resource> observerGetResource = make_shared< restbed::Resource >( );
};
#endif //CARMAGEDDON_OBSERVER_RESOURCE_H
