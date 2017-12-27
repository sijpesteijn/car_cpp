#include <iostream>
#include <list>
#include "rest/rest.h"
#include "rest/camera_resource.h"
#include "rest/car_resource.h"
#include "rest/lifeline_handler.h"
#include "rest/race_resource.h"
#include "util/log.h"

static Camera camera;

void closeResources(void) {
    log::debug(string("Stopping Carmageddon"));
    camera.close();
}

int main( const int, const char** )
{
    log::debug(string("Starting Carmageddon"));
    atexit (closeResources);

    Car car;
    race_resource r_resource(&camera, &car);
    camera_resource camera_res(&camera);
    car_resource car_res(&car);
    lifeline_handler ll_handler(&car);

    Rest rest({&camera_res, &car_res, &ll_handler, &r_resource});
    log::debug(string("Carmageddon started"));
    return EXIT_SUCCESS;
}
