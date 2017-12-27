//
// Created by Gijs Sijpesteijn on 17/11/2017.
//

#include "finish_detection.h"

using namespace cv;
using namespace std;

finish_detection::finish_detection(Camera *camera):observer(camera) {
    this->type = "finish_detection";
    Size dimensions = this->camera->getDimensions();
    this->roi = Rect(0, 0, dimensions.width, dimensions.height);
}

json_t* finish_detection::getJson(bool full) {
    json_t *root = json_object();
    json_object_set_new( root, "type", json_string( this->type.c_str() ) );
    if (full) {
        json_object_set_new(root, "active", json_boolean(this->active));
    }
    json_t* roi = json_object();
    json_object_set_new( roi, "x", json_real( this->roi.x ) );
    json_object_set_new( roi, "y", json_real( this->roi.y ) );
    json_object_set_new( roi, "height", json_real( this->roi.height ) );
    json_object_set_new( roi, "width", json_real( this->roi.width ) );
    json_object_set_new( root, "roi", roi);

    return root;
}

int finish_detection::updateWithJson(json_t* root) {
    this->condition_achieved = json_boolean_value(json_object_get(root, "condition_achieved"));
    this->active = json_boolean_value(json_object_get(root, "active"));
    json_t* roi = json_object_get(root, "roi");
    this->roi.x = json_real_value(json_object_get(roi, "x"));
    this->roi.y = json_real_value(json_object_get(roi, "y"));
    this->roi.width = json_real_value(json_object_get(roi, "width"));
    this->roi.height = json_real_value(json_object_get(roi, "height"));
    return 0;
}

observer* finish_detection::processSnapshot(Mat snapshot) {
//    cout << "Finish detection" << endl;
    return this;
}

void finish_detection::setActive(bool active) {
    this->active = active;
    this->condition_achieved = false;
}