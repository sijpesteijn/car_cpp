//
// Created by Gijs Sijpesteijn on 17/11/2017.
//

#include "finish_detection.h"

using namespace cv;
using namespace std;

finish_detection::finish_detection(Camera *camera, observer* next_observer) {
    this->camera = camera;
    this->nextObserver = next_observer;
    this->type = "finish_detection";
    this->roi = Rect(0, 0, camera->getDimensions().width, camera->getDimensions().height);
}

string finish_detection::getJson(void) {
    json_t *root = json_object();
    json_object_set_new( root, "type", json_string( this->type ) );
    json_object_set_new( root, "active", json_integer( this->active ) );
    json_t* roi = json_object();
    json_object_set_new( roi, "x", json_integer( this->roi.x ) );
    json_object_set_new( roi, "y", json_integer( this->roi.y ) );
    json_object_set_new( roi, "height", json_integer( this->roi.height ) );
    json_object_set_new( roi, "width", json_integer( this->roi.width ) );
    json_object_set_new( root, "roi", roi);
    string dump = json_dumps(root, 0);
    json_decref(root);
    return dump;
}

int finish_detection::updateWithJson(json_t* root) {
    this->condition_achieved = json_integer_value(json_object_get(root, "condition_achieved"));
    this->active = json_integer_value(json_object_get(root, "active"));
    json_t* roi = json_object_get(root, "roi");
    this->roi.x = json_integer_value(json_object_get(roi, "x"));
    this->roi.y = json_integer_value(json_object_get(roi, "y"));
    this->roi.width = json_integer_value(json_object_get(roi, "width"));
    this->roi.height = json_integer_value(json_object_get(roi, "height"));
    return 0;
}

observer* finish_detection::processSnapshot(Mat snapshot) {
    return this;
}