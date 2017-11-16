#include <syslog.h>
#include <iostream>
#include <list>
#include "cpu.h"
#include "rest/rest.h"
#include "rest/observer_handler.h"
#include "rest/camera_resource.h"
#include "rest/car_resource.h"
#include "rest/observer_status_handler.h"
#include "rest/lifeline_handler.h"

static Camera camera;

void closeResources(void) {
    camera.close();
}

int main( const int, const char** )
{
    openlog ("Carmageddon", LOG_PID | LOG_CONS | LOG_NDELAY | LOG_NOWAIT, LOG_LOCAL0);
    setlogmask(LOG_UPTO(LOG_DEBUG));
    syslog (LOG_INFO, "%s", "Starting Carmageddon");
    atexit (closeResources);

    Car car;

    camera_resource camera_res(&camera);
    car_resource car_res(&car);
    lifeline_handler ll_handler(&car);
    observer_status_handler os_handler;
    cpu *carmageddon = new cpu(&camera, &car, &os_handler);
    observer_handler obsvr_handler(carmageddon);

    Rest rest({&camera_res, &car_res, &obsvr_handler, &os_handler, &ll_handler});
    return EXIT_SUCCESS;
}
