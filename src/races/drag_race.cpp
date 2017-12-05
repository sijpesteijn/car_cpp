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
    finish_detection *fd = new finish_detection(this->camera, NULL);
    lane_detection *ld = new lane_detection(this->camera, fd);
    traffic_light *tl = new traffic_light(this->camera, ld);
    this->obs = tl;
    // HIER
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

std::string drag_race::getJson() {
    string body = "{\"name\":\"" + string(this->name) + "\"," +
                "\"running\": " + to_string(this->running) + ", \"observers\": [";
    for (auto const& i : this->getObservers()) {
        string json = i->getJson();
        body += json + ",";
    }
    if (body.size() > 10) {
        body = body.substr(0, body.size() - 1);
    }
    body += "]}";
    return body;
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

void drag_race::updateWithJson(json_t *json) {
    int wasRunning = this->running;
    this->running = json_integer_value(json_object_get(json, "running"));
    json_t *observers= json_object_get(json, "observers");
    for(int i = 0; i< json_array_size(observers); i++) {
        json_t *observer_json = json_array_get(observers, i);
        json_t *type_json = json_object_get(observer_json, "type");
        string type = json_string_value(type_json);
        observer *to_find = this->findObserver(type);
        if (to_find == NULL) {
            syslog(LOG_ERR, "CPU: Could not find observer %s in race.", type.c_str());
        } else {
            to_find->updateWithJson(observer_json);
        }
    }
    if (wasRunning == 1 && this->running == 0) {
        cout << "Stop running" << endl;
        resetAllObservers();
    } else if (wasRunning == 0 && this->running == 1) {
        cout << "Start running" << endl;
        resetAllObservers();
        this->obs->setActive(1);
    }
}

void drag_race::resetAllObservers() {
    for (auto const& i : this->getObservers()) {
        i->setActive(0);
        i->setConditionAchieved(0);
    }
}
