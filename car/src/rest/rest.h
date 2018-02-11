//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_REST_H
#define CARMAGEDDON_REST_H

#include <memory>
#include <restbed>
#include <string>
#include <list>
#include "abstract_rest_resource.h"

class rest {
public:
    rest(std::list<abstract_rest_resource*> resources);
private:
    restbed::Service service;
};

#endif //CARMAGEDDON_REST_H
