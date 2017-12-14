//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#include "lane_detection.h"
#include <math.h>

using namespace cv;
using namespace std;

lane_detection::lane_detection(Camera* camera, observer* next_observer) {
    this->camera = camera;
    this->nextObserver = next_observer;
    this->type = "lane_detection";
    this->roi = Rect(0, 0, camera->getDimensions().width, camera->getDimensions().height);
}

json_t* lane_detection::getJson(void) {
    json_t *root = json_object();
    json_object_set_new( root, "type", json_string( this->type ) );
    json_object_set_new( root, "order", json_integer( this->order ) );
	json_object_set_new( root, "condition_achieved", json_integer( this->condition_achieved ) );
    json_object_set_new( root, "active", json_integer( this->active ) );
    json_object_set_new( root, "threshold1", json_real( this->threshold1 ) );
    json_object_set_new( root, "threshold2", json_real( this->threshold2 ) );
    json_object_set_new( root, "apertureSize", json_integer( this->apertureSize ) );
    json_t* roi = json_object();
    json_object_set_new( roi, "x", json_integer( this->roi.x ) );
    json_object_set_new( roi, "y", json_integer( this->roi.y ) );
    json_object_set_new( roi, "height", json_integer( this->roi.height ) );
    json_object_set_new( roi, "width", json_integer( this->roi.width ) );
    json_object_set_new( root, "roi", roi);
    return root;
}

int lane_detection::updateWithJson(json_t* root) {
    this->condition_achieved = json_integer_value(json_object_get(root, "condition_achieved"));
    this->active = json_integer_value(json_object_get(root, "active"));
    this->order = json_integer_value(json_object_get(root, "order"));
    json_t* roi = json_object_get(root, "roi");
    this->roi.x = json_integer_value(json_object_get(roi, "x"));
    this->roi.y = json_integer_value(json_object_get(roi, "y"));
    this->roi.width = json_integer_value(json_object_get(roi, "width"));
    this->roi.height = json_integer_value(json_object_get(roi, "height"));

    this->threshold1 = json_real_value(json_object_get(root, "threshold1"));
    cout << "TH " << this->threshold1 << endl;
    this->threshold2 = json_real_value(json_object_get(root, "threshold2"));
    this->apertureSize = json_integer_value(json_object_get(root, "apertureSize"));
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
    cout << "Me" << endl;
    this->roi = this->verifyRoi();
    Mat roiSnapshot = snapshot(this->roi);
    Mat blurred, gaussian, canny, cvt;
    blur(roiSnapshot, blurred, Size(3,3), Point(-1,-1));
    imwrite("blurred.jpg", blurred);
    GaussianBlur(blurred, gaussian, Size(3,3), 0, 0);
    imwrite("gaussian.jpg", gaussian);

    Canny(gaussian, canny, this->threshold1, this->threshold2, this->apertureSize);
    imwrite("canny.jpg", canny);
    cvtColor(canny, cvt, CV_GRAY2BGR);

    vector<Vec4i> lines;
    HoughLinesP(canny, lines, 1, CV_PI/180, 50, 50, 10 );
    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        float angle = atan2(l[1] - l[3], l[0] - l[2]);
        float degree = abs(angle * 180 / M_PI);
        if (degree > 45 && degree < 135) {
            line(cvt, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
        } else {
            line(cvt, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 255, 0), 3, CV_AA);
        }
    }
    imwrite("cvt.jpg", cvt);
    return this;
}