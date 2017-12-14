//
// Created by Gijs Sijpesteijn on 18/11/2017.
//

#include "track_race.h"
using namespace std;

track_race::track_race() {
    this->name = "track";
}

list<observer*> track_race::getObservers() {
    return this->observers;
}

json_t* track_race::getJson() {
    json_t *root = json_object();
    json_object_set_new( root, "name", json_string( this->name ) );
    return root;
}

void track_race::updateWithJson(json_t *json, int start) {

}

void track_race::saveSettings() {
    this->sett->save(this->getJson());
}