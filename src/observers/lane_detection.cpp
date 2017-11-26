//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#include "lane_detection.h"

using namespace cv;
using namespace std;

lane_detection::lane_detection(Camera* camera, observer* next_observer) {
    this->camera = camera;
    this->nextObserver = next_observer;
    this->type = "lane_detection";
    this->roi = Rect(0, 0, camera->getDimensions().width, camera->getDimensions().height);
}

string lane_detection::getJson(void) {
    json_t *root = json_object();
    json_object_set_new( root, "type", json_string( this->type ) );
    json_object_set_new( root, "order", json_integer( this->order ) );
//	json_object_set_new( root, "condition_achieved", json_integer( this->condition_achieved ) );
    json_object_set_new( root, "active", json_integer( this->active ) );
    json_t* roi = json_object();
    json_object_set_new( roi, "x", json_integer( this->roi.x ) );
    json_object_set_new( roi, "y", json_integer( this->roi.y ) );
    json_object_set_new( roi, "height", json_integer( this->roi.height ) );
    json_object_set_new( roi, "width", json_integer( this->roi.width ) );
    json_object_set_new( root, "roi", roi);
//	json_object_set_new( root, "pixel_difference", json_integer( this->pixel_difference ) );
//	json_object_set_new( root, "current_pixel_difference", json_integer( this->current_pixel_difference ) );

    string dump = json_dumps(root, 0);
    json_decref(root);
    return dump;
}

int lane_detection::updateWithJson(json_t* root) {
    this->condition_achieved = json_integer_value(json_object_get(root, "condition_achieved"));
//	this->pixel_difference = json_integer_value(json_object_get(root, "pixel_difference"));
//	this->current_pixel_difference = json_integer_value(json_object_get(root, "current_pixel_difference"));
    this->active = json_integer_value(json_object_get(root, "active"));
    this->order = json_integer_value(json_object_get(root, "order"));
    json_t* roi = json_object_get(root, "roi");
    this->roi.x = json_integer_value(json_object_get(roi, "x"));
    this->roi.y = json_integer_value(json_object_get(roi, "y"));
    this->roi.width = json_integer_value(json_object_get(roi, "width"));
    this->roi.height = json_integer_value(json_object_get(roi, "height"));
    return 0;
}

Rect lane_detection::verifyRoi() {
    Size size = this->camera->getDimensions();
    if (this->roi.x + this->roi.width > size.width) {
        if (this->roi.x > size.width) {
            this->roi.x = 0;
            this->roi.width = size.width;
        } else {
            this->roi.width = size.width - this->roi.x;
        }
    }
    if (this->roi.y + this->roi.height > size.height) {
        if (this->roi.y > size.height) {
            this->roi.y = 0;
            this->roi.height = size.height;
        } else {
            this->roi.height = size.height - this->roi.y;
        }
    }
    return this->roi;
}

observer* lane_detection::processSnapshot(Mat snapshot) {
    return this;
}