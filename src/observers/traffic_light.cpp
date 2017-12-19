//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#include "traffic_light.h"
#include <iostream>
#include <string>
#include <syslog.h>
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;
using namespace std;

pthread_mutex_t snapshot_lock = PTHREAD_MUTEX_INITIALIZER;

traffic_light::traffic_light(Camera* camera, observer* next_observer) {
    this->camera = camera;
    this->nextObserver = next_observer;
    this->type = "traffic_light";
    this->pixel_difference = 70;
    this->roi = Rect(0, 0, camera->getDimensions().width, camera->getDimensions().height);
}

json_t* traffic_light::getJson(void) {
    this->verifyRoi();
    json_t *root = json_object();
    json_object_set_new( root, "type", json_string( this->type ) );
    json_object_set_new( root, "condition_achieved", json_real( this->condition_achieved ) );
    json_object_set_new( root, "active", json_real( this->active ) );
    json_t* roi = json_object();
    json_object_set_new( roi, "x", json_real( this->roi.x ) );
    json_object_set_new( roi, "y", json_real( this->roi.y ) );
    json_object_set_new( roi, "height", json_real( this->roi.height ) );
    json_object_set_new( roi, "width", json_real( this->roi.width ) );
    json_object_set_new( root, "roi", roi);
    json_object_set_new( root, "pixel_difference", json_real( this->pixel_difference ) );
    json_object_set_new( root, "current_pixel_difference", json_real( this->current_pixel_difference ) );

    return root;
}

int traffic_light::updateWithJson(json_t* root) {
    this->condition_achieved = json_real_value(json_object_get(root, "condition_achieved"));
    this->pixel_difference = json_real_value(json_object_get(root, "pixel_difference"));
    this->current_pixel_difference = json_real_value(json_object_get(root, "current_pixel_difference"));
    this->active = json_real_value(json_object_get(root, "active"));
    json_t* roi = json_object_get(root, "roi");
    this->roi.x = json_real_value(json_object_get(roi, "x"));
    this->roi.y = json_real_value(json_object_get(roi, "y"));
    this->roi.width = json_real_value(json_object_get(roi, "width"));
    this->roi.height = json_real_value(json_object_get(roi, "height"));
    return 0;
}

observer* traffic_light::processSnapshot(Mat snapshot) {
//    cout << "Working" << endl;
    this->roi = this->verifyRoi();
    Mat roiSnapshot = snapshot(this->roi);

//    cout << "ss: " << roiSnapshot.size() << endl;
//    cout << "ls: " << this->last_snapshot.size() << endl;
    if (this->condition_achieved == 0 && !this->last_snapshot.empty()
        && this->last_snapshot.size() == roiSnapshot.size()) {
        if (pthread_mutex_lock(&snapshot_lock) != 0) {
            syslog(LOG_ERR, "%s", "Could not get a lock on the snapshot lock");
        }
//        cout << "x: " << this->roi.x << " w: " << this->roi.width << " y: " << this->roi.y << " h: " << this->roi.height << endl;
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
        if (percentage >= this->pixel_difference) {
            this->condition_achieved = 1;
        }
        if (percentage > 0) {
            cout << "Difference: " << to_string(percentage) << "%" << endl;
        }
        if (pthread_mutex_unlock(&snapshot_lock) != 0) {
            syslog(LOG_ERR, "%s", "Could not unlock on the snapshot lock");
        }
    } else {
        this->last_snapshot = roiSnapshot.clone();
    }
    return this;
}

Rect traffic_light::verifyRoi() {
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
