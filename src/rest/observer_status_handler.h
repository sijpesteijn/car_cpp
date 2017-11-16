//
// Created by Gijs Sijpesteijn on 09/11/2017.
//

#ifndef CARMAGEDDON_OBSERVER_STATUS_HANDLER_H
#define CARMAGEDDON_OBSERVER_STATUS_HANDLER_H

#include <restbed>
#include <memory>
#include <list>
#include "carmageddon_resource.h"
#include "../domain/observer.h"

class observer_status_handler: public carmageddon_resource {
public:
    observer_status_handler();
    std::list<std::shared_ptr<restbed::Resource>> getResources();
    void notifyClients(std::list<observer *> list);
private:
    std::shared_ptr<restbed::Resource> observerStatusResource = std::make_shared< restbed::Resource >( );
};


#endif //CARMAGEDDON_OBSERVER_STATUS_HANDLER_H
