//
// Created by Gijs Sijpesteijn on 18/11/2017.
//

#include "drag_race.h"
#include "../observers/traffic_light.h"
#include "../observers/lane_detection.h"
#include "../observers/finish_detection.h"
#include "../util/util.h"
#include <syslog.h>

using namespace std;
using namespace restbed;
using namespace cv;

drag_race::drag_race(Camera *camera) {
    this->name = "drag";
    this->camera = camera;
    this->sett = new settings("../src/drag_race.json");
    finish_detection *fd = new finish_detection(this->camera, NULL);
    lane_detection *ld = new lane_detection(this->camera, fd);
    traffic_light *tl = new traffic_light(this->camera, ld);
    this->obs = tl;
    this->updateWithJson(sett->getSettings(), 0);
    // wait for the camera
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    pthread_t observer_runner;
    pthread_create(&observer_runner, NULL, checkObservers, this);
}

list<observer*> drag_race::getObservers() {
    list<observer*> observers;
    observer *curr = this->obs;
    while(curr) {
        observers.push_back(curr);
        curr = curr->nextObserver;
    }
    return observers;
}

json_t* drag_race::getJson() {
    json_t *root = json_object();
    json_object_set_new( root, "name", json_string( this->name ) );
    json_object_set_new( root, "running", json_integer( this->running ) );
    json_t* observers = json_array();
    for (auto const& i : this->getObservers()) {
        json_array_append(observers, i->getJson());
    }
    json_object_set_new( root, "observers", observers );
    return root;
}

observer* drag_race::findObserver(string type) {
    observer *curr = this->obs;
    while (curr) {
        if (type.compare(string(curr->getType())) == 0) {
            return curr;
        } else {
            curr = curr->nextObserver;
        }
    }
    return NULL;
}

void drag_race::updateWithJson(json_t *json, int start) {
    int wasRunning = this->running;
    this->running = json_integer_value(json_object_get(json, "running"));
    json_t *observers= json_object_get(json, "observers");
    for(int i = 0; i< json_array_size(observers); i++) {
        json_t *observer_json = json_array_get(observers, i);
        json_t *type_json = json_object_get(observer_json, "type");
        string type = json_string_value(type_json);
        observer *to_find = this->findObserver(type);
        if (to_find == NULL) {
            cout << "Drag race: Could not find observer " << type.c_str() << " in race." << endl;
            syslog(LOG_ERR, "CPU: Could not find observer %s in race.", type.c_str());
        } else {
            to_find->updateWithJson(observer_json);
        }
    }
    if (start == 1) {
        if (wasRunning == 1 && this->running == 0) {
            cout << "Stop running" << endl;
            resetAllObservers();
        } else if (wasRunning == 0 && this->running == 1) {
            cout << "Start running" << endl;
            resetAllObservers();
            this->obs->setActive(1);
        }
    } else {
        this->running = 0;
        resetAllObservers();
    }
}

void drag_race::resetAllObservers() {
    observer *curr = this->obs;
    while(curr) {

//        for (auto const& i : this->getObservers()) {
        curr->setActive(0);
        curr->setConditionAchieved(0);
        curr = curr->nextObserver;
    }
}

void drag_race::saveSettings() {
    this->sett->save(this->getJson());
}