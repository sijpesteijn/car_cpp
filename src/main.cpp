//
// Created by Gijs Sijpesteijn on 21/01/2018.
//

#include "util/log.h"
#include "domain/car.h"
#include "domain/camera.h"
#include "rest/rest.h"
#include "rest/race_resource.h"
#include "rest/camera_resource.h"
#include <string>
#include <thread>
#include <iostream>
#include <syslog.h>
#include <curl/curl.h>

using namespace std;

static Car* car;
static Camera* camera;

void closeResources(void) {
    log::debug(string("Stopping Carmageddon - CPU"));
}

size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
    data->clear();
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}

void *cpuCarStatus(void *params) {
    std::string response_string;
    std::string header_string;
    long response_code;
    CURL* curl; //our curl object

    curl_global_init(CURL_GLOBAL_ALL); //pretty obvious
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, "http://carmageddon.test/car/mode");
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    bool run = true;
    while(run) {
        curl_easy_perform(curl);
        car->setMode(static_cast<car_mode >(atoi(response_string.c_str())));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    return NULL;
}

int main( const int argc, const char* argv[])
{
#ifdef __APPLE__
    openlog ("Carmageddon - CPU", LOG_CONS,LOG_USER);
#endif
    log::debug(string("Starting Carmageddon - CPU"));
    car = new Car();
    atexit (closeResources);
    if (argc < 2) {
        camera = new Camera(true);
    } else {
        camera = new Camera(strcmp(argv[1], "true") == 1 ? false : true);
    }

    pthread_t carstatus;
    pthread_create(&carstatus, NULL, cpuCarStatus, NULL);

    race_resource r_resource(camera, car);
    camera_resource c_resource(camera);
    rest r({&r_resource, &c_resource});

    log::debug(string("Carmageddon - CPU started"));

    pthread_join(carstatus, NULL);
    return EXIT_SUCCESS;
}
