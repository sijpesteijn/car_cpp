#include <iostream>
#include <list>
#include <string>
#include <syslog.h>
#include "rest/rest.h"
#include "rest/car_resource.h"
#include "rest/lifeline_resource.h"
#include "util/log.h"
#include "domain/car.h"

using namespace std;

static Car car;

void closeResources(void) {
    log::debug(string("Stopping Carmageddon - Car"));
}

int main( const int, const char** )
{
#ifdef __APPLE__
    openlog ("Carmageddon - Car", LOG_CONS,LOG_USER);
#endif
    log::debug(string("Starting Carmageddon - Car"));
    atexit (closeResources);

    car_resource c_resource(&car);
    lifeline_resource ll_resource(&car);

    rest r({&c_resource, &ll_resource});
    log::debug(string("Carmageddon - Car started"));
    return EXIT_SUCCESS;
}
