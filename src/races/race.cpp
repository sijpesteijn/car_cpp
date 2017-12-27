//
// Created by Gijs Sijpesteijn on 22/12/2017.
//


#include "race.h"
#include "../util/util.h"
#include <iostream>
#include <chrono>
#include <string>

using namespace std;

race::race(string name, Car *car, Camera *camera, settings *settings, observer_group *group) {
    this->name = name;
    this->camera = camera;
    this->car = car;
    this->sett = settings;
    this->group = group;
}

void race::setSelected(bool selected) {
    if (this->selected != selected) {
        this->selected = selected;
        if (this->selected) {
            pthread_create(&this->observer_runner, NULL, checkObservers, this);
            this->group->setOutputDir(string("./")
                                              .append(this->name)
                                              .append("/")
                                              .append(to_string(std::chrono::system_clock::now().time_since_epoch().count()))
                                              .append("/"));
        }
    }
    if (!this->selected) {
        pthread_cancel(this->observer_runner);
    }
}

void updateGroup(json_t *json, observer_group *group) {
    json_t *groupJson = json_object_get(json, "group");
    json_t *observers= json_object_get(groupJson, "observers");
    for(int i = 0; i< json_array_size(observers); i++) {
        json_t *observer_json = json_array_get(observers, i);
        json_t *type_json = json_object_get(observer_json, "type");
        string type = json_string_value(type_json);
        group->name = json_string_value(json_object_get(groupJson, "name"));
        observer *observer = group->getObserver(type);
        if (observer) {
            observer->updateWithJson(observer_json);
        }
    }
    if (group->next) {
        updateGroup(groupJson, group->next);
    }
}

void race::updateWithJson(json_t *json, bool start) {
    this->name = json_string_value(json_object_get(json, "name"));
    if (start && json_object_get(json, "running")) {
        bool wasRunning = this->running;
        this->running = json_boolean_value(json_object_get(json, "running"));
        if (wasRunning && !this->running) {
            cout << "Stop running" << endl;
            this->group->setActive(false);
            this->group->reset();
        } else if (wasRunning == 0 && this->running == 1) {
            cout << "Start running" << endl;
            this->group->setActive(true);
        }
    }
    if (!start && this->running) {
        this->running = false;
        this->group->setActive(false);
    }
    updateGroup(json, this->group);
}

json_t *getGroup(observer_group *grp, bool full) {
    json_t *group = json_object();
    json_t *observers = json_array();
    for (auto const &i : grp->observers) {
        json_array_append(observers, i->getJson(full));
    }
    json_object_set_new( group, "name", json_string(grp->name.c_str()));
    json_object_set_new( group, "observers", observers );
    if (grp->next) {
        json_object_set_new(group, "group", getGroup(grp->next, full));
    }
    return group;
}

json_t* race::getJson(bool full) {
    json_t *root = json_object();
    json_object_set_new( root, "name", json_string( this->name.c_str() ) );
    if (full) {
        json_object_set_new(root, "running", json_boolean(this->running));
    }
    observer_group *grp = this->group;
    json_object_set_new(root, "group", getGroup(grp, full));

    return root;
}

void race::saveSettings() {
    this->sett->save(this->getJson());
}