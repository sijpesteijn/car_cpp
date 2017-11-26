//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#include "cpu.h"
#include "./observers/traffic_light.h"
#include "./observers/lane_detection.h"
#include "observers/finish_detection.h"
#include <syslog.h>
#include <unistd.h>

using namespace cv;

//void* checkObservers(void* params) {
//    cpu *carmageddon = (cpu*) params;
//    while(1) {
//        Mat snapshot = carmageddon->camera->getFrame();
////        list<observer*> observers = {};
//        for (auto const& i : carmageddon->getObservers()) {
//            if (i->isActive()) {
//                i->processSnapshot(snapshot);
////                observers.push_back(i->processSnapshot(snapshot));
//            }
//        }
//        if (carmageddon->car->getMode() == car_mode::autonomous) {
//            carmageddon->os_handler->notifyClients(carmageddon->getObservers());
//        }
//        std::this_thread::sleep_for(std::chrono::milliseconds(carmageddon->camera->observers_delay));
//    }
//    return NULL;
//}

cpu::cpu(Camera *camera, Car *car, observer_status_handler *os_handler) {
    this->camera = camera;
    this->car = car;
    this->os_handler = os_handler;
    finish_detection *fd = new finish_detection(this->camera, NULL);
    lane_detection *ld = new lane_detection(this->camera, fd);
    traffic_light *tl = new traffic_light(this->camera, ld);
    this->observers.insert(pair<string, observer*>(tl->getType(), tl));
    this->observers.insert(pair<string, observer*>(ld->getType(), ld));
    this->observers.insert(pair<string, observer*>(fd->getType(), fd));
//    pthread_t observer_runner;
//    pthread_create(&observer_runner, NULL, checkObservers, this);
}

list<observer*> cpu::getObservers() {
    list<observer*> observers = {};
    for(map<string,observer*>::iterator it = this->observers.begin(); it != this->observers.end(); ++it) {
        observers.push_back(it->second);
    }
    return observers;
}

observer* cpu::getObserver(string name) {
    map<string,observer*>::iterator it = this->observers.find(name);
    if (it == this->observers.end()) {
        syslog(LOG_ERR, "CPU: Could not get observer %s.", name.c_str());
        return NULL;
    } else {
        return it->second;
    }
}