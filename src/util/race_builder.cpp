//
// Created by Gijs Sijpesteijn on 22/12/2017.
//

#include <dirent.h>
#include "race_builder.h"
#include "../observers/lane_detection.h"
#include "../observers/finish_detection.h"
#include "../observers/traffic_light.h"

using namespace std;

race_builder::race_builder() {

}

list<struct dirent*> listFiles(string path) {
    list<struct dirent*> files;
    DIR* dirFile = opendir( path.c_str() );
    if ( dirFile ) {
        struct dirent *hFile;
        errno = 0;
        while ((hFile = readdir(dirFile)) != NULL) {
            if (!strcmp(hFile->d_name, ".")) continue;
            if (!strcmp(hFile->d_name, "..")) continue;
            if (hFile->d_name[0] == '.') continue;
            if (strstr(hFile->d_name, ".json")) {
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
    for (int i = 0; i < json_array_size(observers_json); i++) {
        json_t *observer_json = json_array_get(observers_json, i);
        json_t *type_json = json_object_get(observer_json, "type");
        string type = json_string_value(type_json);
        if (type.compare("lane_detection") == 0) {
            lane_detection *ld = new lane_detection(camera);
            ld->updateWithJson(observer_json);
            observers.push_back(ld);
        } else if(type.compare("finish_detection") == 0) {
            finish_detection *fl = new finish_detection(camera);
            fl->updateWithJson(observer_json);
            observers.push_back(fl);
        } else if (type.compare("traffic_light") == 0) {
            traffic_light *tl = new traffic_light(camera);
            tl->updateWithJson(observer_json);
            observers.push_back(tl);
        }
    }
    return observers;
}
observer_group *race_builder::getObserverGroup(json_t* root, Camera *camera) {
    json_t *child_group_json = json_object_get(root, "group");
    string name = json_string_value(json_object_get(root, "name"));
    observer_group *child_group = NULL;
    if (child_group_json) {
        child_group = this->getObserverGroup(child_group_json, camera);
    }
    observer_group *group = new observer_group(name, this->getObservers(root, camera));
    group->next = NULL;
    if (child_group && (!child_group->observers.empty() || child_group->next)) {
        group->next = child_group;
    }
    return group;
}

map<string, race*> race_builder::getRaces(string path, Car *car, Camera* camera) {
    map<string, race*> races;
    list<struct dirent*> files = listFiles(path);
    for(const auto& i : files) {
        settings *sett = new settings(string(path).append(i->d_name));
        json_t* json = sett->getSettings();
        json_t *name_json = json_object_get(json, "name");
        string name = json_string_value(name_json);

        json_t *group_json = json_object_get(json, "group");
        observer_group *group = this->getObserverGroup(group_json, camera);
        race *r = new race(name, car, camera, sett, group);
        races.insert(pair<string, race*>(r->getName(), r));
    }
    return races;
}
