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

string track_race::getJson() {
    json_t *root = json_object();
    json_object_set_new( root, "name", json_string( this->name ) );
    string dump = json_dumps(root, 0);
    json_decref(root);
    return dump;
}

void track_race::updateWithJson(json_t *json) {

}