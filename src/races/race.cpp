//
// Created by Gijs Sijpesteijn on 22/12/2017.
//


#include "race.h"
#include "../util/util.h"

using namespace std;

race::race(string name, Car *car, Camera *camera, settings *settings, list<observer_group *> groups) {
    this->name = std::move(name);
    this->camera = camera;
    this->car = car;
    this->sett = settings;
    this->groups = groups;
    for(observer_group *group: groups) {
        group->setOutputDir(string("results/").append(this->name).append("/"));
    }
}

void race::setSelected(bool selected) {
    if (this->selected != selected) {
        this->selected = selected;
        for(observer_group *group: groups) {
            group->setSelected(this->selected);
        }
        if (this->selected) {
            pthread_create(&this->observer_runner, nullptr, checkObservers, this);
        }
    }
    if (!this->selected) {
        pthread_cancel(this->observer_runner);
    }
}

bool race::isSelected() {
    return this->selected;
}

void updateGroup(json_t *json, observer_group *group) {
    group->name = json_string_value(json_object_get(json, "name"));

    json_t *observers = json_object_get(json, "observers");
    for (size_t i = 0; i < json_array_size(observers); i++) {
        json_t *observer_json = json_array_get(observers, i);
        json_t *type_json = json_object_get(observer_json, "type");
        string type = json_string_value(type_json);
        observer *observer = group->getObserver(type);
        if (observer) {
            observer->updateWithJson(observer_json);
        }
    }
}

void race::updateWithJson(json_t *json, bool start) {
    this->name = json_string_value(json_object_get(json, "name"));
    json_t *groupsJson = json_object_get(json, "groups");
    for(size_t j = 0; j < json_array_size(groupsJson); j++) {
        json_t *groupJson = json_array_get(groupsJson, j);
        string name = json_string_value(json_object_get(groupJson, "name"));
        observer_group * group = this->getObserverGroup(name);
        updateGroup(groupJson, group);
    }

    if (start && json_object_get(json, "running")) {
        bool wasRunning = this->running;
        this->running = json_boolean_value(json_object_get(json, "running"));
        if (wasRunning && !this->running) {
            cout << "Stop running" << endl;
            for(observer_group *group: this->groups) {
                group->setRunning(false);
            }
        } else if (wasRunning == 0 && this->running == 1) {
            cout << "Start running" << endl;
            for(observer_group *group: this->groups) {
                group->setRunning(true);
            }
        }
    }
    if (!start && this->running) {
        this->running = false;
        for(observer_group *group: this->groups) {
            group->setRunning(false);
        }
    }
}

json_t *getGroup(observer_group *grp, bool full) {
    json_t *group = json_object();
    json_t *observers = json_array();
    for (auto const &i : grp->observers) {
        json_array_append(observers, i->getJson(full));
    }
    json_object_set_new( group, "name", json_string(grp->name.c_str()));
    json_object_set_new( group, "observers", observers );
    return group;
}

json_t* race::getJson(bool full) {
    json_t *root = json_object();
    json_object_set_new( root, "name", json_string( this->name.c_str() ) );
    if (full) {
        json_object_set_new(root, "running", json_boolean(this->running));
    }
    json_t *groupsJson = json_array();
    for(observer_group *group: groups) {
        json_t *groupJson = getGroup(group, full);
        json_array_append(groupsJson, groupJson);
    }
    json_object_set_new(root, "groups", groupsJson);

    return root;
}

void race::saveSettings() {
    this->sett->save(this->getJson(false));
}

void race::setRunning(bool running) {
    this->running = running;
    for(observer_group *group: this->groups) {
        group->setRunning(running);
    }
}

bool race::isRunning() {
    return this->running;
}

string race::getPreviewImageLocation(const string group_name, const string observer_name, const string observer_stage) {
    for (observer_group *group: this->groups) {
        if (group->name == group_name) {
            for (observer *observer : group->observers) {
                if (observer->getType() == observer_name) {
                    return observer->getPreviewImageLocation(observer_stage);
                }
            }
        }
    }
    return std::string();
}

observer_group *race::getObserverGroup(string name) {
    for (observer_group *grp : this->groups) {
        if (grp->name == name) {
            return grp;
        }
    }
    return nullptr;
}
