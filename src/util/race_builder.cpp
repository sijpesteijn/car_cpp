//
// Created by Gijs Sijpesteijn on 22/12/2017.
//

#include <dirent.h>
#include "race_builder.h"
#include "../observers/lane_detection.h"
#include "../observers/finish_detection.h"
#include "../observers/traffic_light.h"
#include "log.h"

using namespace std;

race_builder::race_builder() = default;

list<struct dirent*> listFiles(const string &path) {
    list<struct dirent*> files;
    DIR* dirFile = opendir( path.c_str() );
    if ( dirFile ) {
        struct dirent *hFile;
        errno = 0;
        while ((hFile = readdir(dirFile)) != nullptr) {
            if (!strcmp(hFile->d_name, ".")) continue;
            if (!strcmp(hFile->d_name, "..")) continue;
            if (hFile->d_name[0] == '.') continue;
            if (strstr(hFile->d_name, ".json")) {
                log::debug(string("Found ").append(hFile->d_name));
                files.push_back(hFile);
            }
        }
        closedir(dirFile);
    }
    return files;
}

list<observer*> race_builder::getObservers(json_t* root, Camera *camera) {
    list<observer*> observers;
    json_t *observers_json = json_object_get(root, "observers");
    for (size_t i = 0; i < json_array_size(observers_json); i++) {
        json_t *observer_json = json_array_get(observers_json, i);
        json_t *type_json = json_object_get(observer_json, "type");
        string type = json_string_value(type_json);
        if (type == "lane_detection") {
            auto *ld = new lane_detection(camera);
            ld->updateWithJson(observer_json);
            observers.push_back(ld);
        } else if(type == "finish_detection") {
            auto *fl = new finish_detection(camera);
            fl->updateWithJson(observer_json);
            observers.push_back(fl);
        } else if (type == "traffic_light") {
            auto *tl = new traffic_light(camera);
            tl->updateWithJson(observer_json);
            observers.push_back(tl);
        }
    }
    return observers;
}
list<observer_group *> race_builder::getObserverGroups(json_t* root, Camera *camera) {
    list<observer_group*> groups;
    for (size_t i = 0; i < json_array_size(root); i++) {
        json_t *groupJson = json_array_get(root, i);
        string name = json_string_value(json_object_get(groupJson, "name"));
        observer_group *group = new observer_group(name, this->getObservers(groupJson, camera));
        groups.push_back(group);
    }
    return groups;
}

map<string, race*> race_builder::getRaces(string path, Car *car, Camera* camera) {
    map<string, race*> races;
    list<struct dirent*> files = listFiles(path);
    for(const auto& i : files) {
        settings *sett = new settings(string(path).append(i->d_name));
        json_t* json = sett->getSettings();

        string name = json_string_value(json_object_get(json, "name"));
        json_t *groups_json = json_object_get(json, "groups");
        list<observer_group *> groups = this->getObserverGroups(groups_json, camera);

        race *r = new race(name, car, camera, sett, groups);
        races.insert(pair<string, race*>(r->getName(), r));
    }
    return races;
}
