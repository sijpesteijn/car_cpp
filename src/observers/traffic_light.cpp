//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#include "traffic_light.h"
#include <iostream>
#include <string>
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "../util/log.h"

using namespace cv;
using namespace std;

pthread_mutex_t snapshot_lock = PTHREAD_MUTEX_INITIALIZER;

traffic_light::traffic_light(Camera* camera):observer(camera) {
    this->type = "traffic_light";
    this->pixel_difference = 70;
    Size dimensions = camera->getDimensions();
    this->roi = Rect(0, 0, dimensions.width, dimensions.height);
}

json_t* traffic_light::getJson(bool full) {
    this->roi = this->verifyRoi(this->roi);
    json_t *root = json_object();
    json_object_set_new( root, "type", json_string( this->type.c_str()) );
    if (full) {
        json_object_set_new(root, "condition_achieved", json_boolean(this->condition_achieved));
        json_object_set_new(root, "active", json_boolean(this->active));
        json_object_set_new(root, "current_pixel_difference", json_real(this->current_pixel_difference));
    }
    json_t* roi = json_object();
    json_object_set_new( roi, "x", json_real( this->roi.x ) );
    json_object_set_new( roi, "y", json_real( this->roi.y ) );
    json_object_set_new( roi, "height", json_real( this->roi.height ) );
    json_object_set_new( roi, "width", json_real( this->roi.width ) );
    json_object_set_new( root, "roi", roi);
    json_object_set_new( root, "pixel_difference", json_real( this->pixel_difference ) );

    return root;
}

int traffic_light::updateWithJson(json_t* root) {
    this->pixel_difference = json_real_value(json_object_get(root, "pixel_difference"));
    json_t* roi = json_object_get(root, "roi");
    this->roi.x = json_real_value(json_object_get(roi, "x"));
    this->roi.y = json_real_value(json_object_get(roi, "y"));
    this->roi.width = json_real_value(json_object_get(roi, "width"));
    this->roi.height = json_real_value(json_object_get(roi, "height"));
    this->roi = this->verifyRoi(this->roi);
    return 0;
}

observer* traffic_light::processSnapshot(Mat snapshot) {
    if (!this->condition_achieved) {
        this->current_pixel_difference = 0;
    }
    this->roi = this->verifyRoi(this->roi);
    Mat roiSnapshot = snapshot(this->roi);

    if (!this->condition_achieved && !this->last_snapshot.empty()
        && this->last_snapshot.size() == roiSnapshot.size()) {
        if (pthread_mutex_lock(&snapshot_lock) != 0) {
            log::debug(string("Could not get a lock on the snapshot lock"));
        }
        Mat result;
        absdiff(roiSnapshot, this->last_snapshot, result);
        cvtColor(result, result, CV_BGR2GRAY);
        float threshold = 30;
        Mat mask = result < threshold;
        int nonZero = countNonZero(mask);
        int percentage = 0;
        if (nonZero > 0 ) {
            percentage = 100 - (int) (nonZero*100/mask.total());
        }
        this->current_pixel_difference = percentage;
        if (this->isActive() && percentage >= this->pixel_difference) {
            this->condition_achieved = true;
        }
        if (percentage > 0) {
            cout << "Traffic_light - difference: " << to_string(percentage) << "%" << endl;
        }
        if (pthread_mutex_unlock(&snapshot_lock) != 0) {
            log::debug(string("Could not unlock on the snapshot lock"));
        }
    } else {
        this->last_snapshot = roiSnapshot.clone();
    }
    return this;
}

void traffic_light::setActive(bool active) {
    this->active = active;
    this->current_pixel_difference = 0;
}