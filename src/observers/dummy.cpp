//
// Created by Gijs Sijpesteijn on 22/12/2017.
//

#include "dummy.h"


dummy::dummy(Camera *camera):observer(camera) {
    this->type = "dummy_detection";
}

observer* dummy::processSnapshot(cv::Mat snapshot) {
    return this;
}

json_t* dummy::getJson(bool full) {
    json_t *root = json_object();
    json_object_set_new( root, "type", json_string( this->type.c_str() ) );
    return root;
}

int dummy::updateWithJson(json_t *root) {

    return 0;
}

void dummy::setActive(bool active) {
    this->active = active;
    this->condition_achieved = false;
}
