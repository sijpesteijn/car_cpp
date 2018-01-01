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
        json_object_set_new(root, "selected", json_boolean(this->selected));
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
//    if (!this->condition_achieved) {
//        this->current_pixel_difference = 0;
//    }
    this->roi = this->verifyRoi(this->roi);
    Mat roiSnapshot = snapshot(this->roi);

    if (!this->condition_achieved && !this->last_snapshot.empty()
        && this->last_snapshot.size() == roiSnapshot.size()) {
        Mat result, mask;
        absdiff(roiSnapshot, this->last_snapshot, result);
        cvtColor(result, result, CV_BGR2GRAY);
        threshold(result, mask, 30, 255.0, CV_THRESH_BINARY);
        int nonZero = countNonZero(mask);
        int percentage = 0;
        if (nonZero > 0 ) {
            percentage = (int) (nonZero*100/mask.total());
        }
        this->current_pixel_difference = percentage;
        if (this->isRunning() && percentage >= this->pixel_difference) {
            this->condition_achieved = true;
        }
        if (percentage > 0) {
            cout << "Traffic_light - difference: " << to_string(percentage) << "%" << endl;
        }
        imwrite("first.jpg", this->last_snapshot);
        this->last_snapshot = roiSnapshot.clone();
        imwrite("sec.jpg", this->last_snapshot);
    } else {
        this->last_snapshot = roiSnapshot.clone();
    }
    return this;
}

void traffic_light::setSelected(bool selected) {
    this->selected = selected;
    this->current_pixel_difference = 0;
}