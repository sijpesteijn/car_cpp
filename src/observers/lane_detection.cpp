//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#include "lane_detection.h"
#include "Line.h"

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
	json_object_set_new( root, "condition_achieved", json_real( this->condition_achieved ) );
    json_object_set_new( root, "active", json_real( this->active ) );
    json_object_set_new( root, "threshold1", json_real( this->threshold1 ) );
    json_object_set_new( root, "threshold2", json_real( this->threshold2 ) );
    json_object_set_new( root, "apertureSize", json_real( this->apertureSize ) );
    json_t* roi = json_object();
    json_object_set_new( roi, "x", json_real( this->roi.x ) );
    json_object_set_new( roi, "y", json_real( this->roi.y ) );
    json_object_set_new( roi, "height", json_real( this->roi.height ) );
    json_object_set_new( roi, "width", json_real( this->roi.width ) );
    json_object_set_new( root, "roi", roi);
    return root;
}

int lane_detection::updateWithJson(json_t* root) {
    this->condition_achieved = json_real_value(json_object_get(root, "condition_achieved"));
    this->active = json_real_value(json_object_get(root, "active"));
    json_t* roi = json_object_get(root, "roi");
    this->roi.x = json_real_value(json_object_get(roi, "x"));
    this->roi.y = json_real_value(json_object_get(roi, "y"));
    this->roi.width = json_real_value(json_object_get(roi, "width"));
    this->roi.height = json_real_value(json_object_get(roi, "height"));

    this->threshold1 = json_real_value(json_object_get(root, "threshold1"));
    this->threshold2 = json_real_value(json_object_get(root, "threshold2"));
    this->apertureSize = json_real_value(json_object_get(root, "apertureSize"));
    this->getJson();
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
    list<Line> vertical;
    list<Line> horizontal;
    HoughLinesP(canny, lines, 1, CV_PI/180, 50, 50, 10 );
//    list<Line> lines;
    Point p1, p2;
    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        if (l[0] > l[2]) {
            p1 = Point(l[2], l[3]);
            p2 = Point(l[0], l[1]);
        } else {
            p1 = Point(l[0], l[1]);
            p2 = Point(l[2], l[3]);
        }

        float angle = atan2(p1.y - p2.y, p1.x - p2.x);
        float degree = abs(angle * 180 / M_PI);
        if (degree > 25 && degree < 155) {
            line(cvt, p1, p2, Scalar(0, 255, 255), 3, CV_AA);
            circle(cvt, p1, 5, Scalar(255, 0, 0), 5);
            circle(cvt, p2, 5, Scalar(0, 0, 255), 5);
            vertical.push_back(Line(p1, p2));
        } else {
            line(cvt, p1, p2, Scalar(0, 255, 0), 3, CV_AA);
            horizontal.push_back(Line(p1, p2));
        }
    }

//    Line average = this->getAverageLine(vertical);
//    line(cvt, average.p1, average.p2, Scalar(255, 255, 255), 3, CV_AA);
    imwrite("cvt.jpg", cvt);
    return this;
}

Line lane_detection::getAverageLine(std::list<Line> lines) {
    int length = lines.size();
    int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
    for (std::list<Line>::iterator it=lines.begin(); it != lines.end(); ++it) {
        x1 += it->p1.x;
        x2 += it->p2.x;
        y1 += it->p1.y;
        y2 += it->p2.y;
    }
//    cout << "x y " << x1/length << " " << y1/length << endl;
    return Line(Point(x1/length, y1/length), Point(x2/length, y2/length));
}