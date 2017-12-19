#include <syslog.h>
#include <iostream>
#include <list>
#include "rest/rest.h"
#include "rest/camera_resource.h"
#include "rest/car_resource.h"
#include "rest/observer_status_handler.h"
#include "rest/lifeline_handler.h"
#include "rest/race_resource.h"

static Camera camera;

void closeResources(void) {
    camera.close();
}

int main( const int, const char** )
{
    openlog ("Carmageddon", LOG_CONS,LOG_USER);
//    setlogmask(LOG_UPTO(LOG_INFO));
    syslog (LOG_NOTICE, "%s", "Starting Carmageddon");
    atexit (closeResources);

    Car car;
    camera_resource camera_res(&camera);
    car_resource car_res(&car);
    lifeline_handler ll_handler(&car);
    observer_status_handler os_handler;
    race_resource r_resource(&camera);

    Rest rest({&camera_res, &car_res, &os_handler, &ll_handler, &r_resource});
    return EXIT_SUCCESS;
}
