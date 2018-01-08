//
// Created by Gijs Sijpesteijn on 17/11/2017.
//

#include "finish_detection.h"
#include "opencv_line.h"

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
        json_object_set_new(root, "condition_achieved", json_boolean(this->condition_achieved));
        json_object_set_new(root, "selected", json_boolean(this->selected));
    }
    json_t* roi = json_object();
    json_object_set_new( roi, "x", json_real( this->roi.x ) );
    json_object_set_new( roi, "y", json_real( this->roi.y ) );
    json_object_set_new( roi, "height", json_real( this->roi.height ) );
    json_object_set_new( roi, "width", json_real( this->roi.width ) );
    json_object_set_new( root, "roi", roi);
    json_object_set_new( root, "threshold1", json_real( this->threshold1 ) );
    json_object_set_new( root, "threshold2", json_real( this->threshold2 ) );
    json_object_set_new( root, "apertureSize", json_real( this->apertureSize ) );
    return root;
}

int finish_detection::updateWithJson(json_t* root) {
    json_t* roi = json_object_get(root, "roi");
    this->roi.x = static_cast<int>(json_real_value(json_object_get(roi, "x")));
    this->roi.y = static_cast<int>(json_real_value(json_object_get(roi, "y")));
    this->roi.width = static_cast<int>(json_real_value(json_object_get(roi, "width")));
    this->roi.height = static_cast<int>(json_real_value(json_object_get(roi, "height")));
    this->threshold1 = json_real_value(json_object_get(root, "threshold1"));
    this->threshold2 = json_real_value(json_object_get(root, "threshold2"));
    this->apertureSize = static_cast<int>(json_real_value(json_object_get(root, "apertureSize")));

    return 0;
}

observer* finish_detection::processSnapshot(Mat snapshot) {
    if (snapshot.total() > 0) {
        this->roi = this->verifyRoi(this->roi);
        Mat roiSnapshot = snapshot(this->roi);
        Mat blurred, gaussian, canny, cvt;
        blur(roiSnapshot, blurred, Size(3, 3), Point(-1, -1));
        writeImage("blurred.jpg", blurred);
        GaussianBlur(blurred, gaussian, Size(3, 3), 0, 0);
        writeImage("gaussian.jpg", gaussian);

        Canny(gaussian, canny, this->threshold1, this->threshold2, this->apertureSize);
        writeImage("canny.jpg", canny);
        cvtColor(canny, cvt, CV_GRAY2BGR);

        vector<Vec4i> lines;
        list<opencv_line> horizontal;
        HoughLinesP(canny, lines, 1, CV_PI / 180, 50, 50, 10);
        Point p1, p2;
        for (auto l : lines) {
            if (l[0] > l[2]) {
                p1 = Point(l[2], l[3]);
                p2 = Point(l[0], l[1]);
            } else {
                p1 = Point(l[0], l[1]);
                p2 = Point(l[2], l[3]);
            }

            auto angle = static_cast<float>(atan2(p1.y - p2.y, p1.x - p2.x));
            auto degree = static_cast<float>(abs(angle * 180 / M_PI));
            if (degree <= 25 || degree >= 155) {
                line(cvt, p1, p2, Scalar(0, 255, 0), 3, CV_AA);
                horizontal.emplace_back(p1, p2);
            }
        }
        writeImage("cvt.jpg", cvt);
    }
    return this;
}

void finish_detection::setSelected(bool selected) {
    this->selected = selected;
    if (this->selected) {
        this->condition_achieved = false;
        struct stat info;
        if (stat(this->outputDir.c_str(), &info) != 0) {
            std::cout << "making " << this->outputDir << std::endl;
            _mkdir(this->outputDir.c_str());
        }
    }
}

void finish_detection::setRunning(bool running) {
    this->running = running;
    if (!this->running) {
        this->condition_achieved = false;
    }
}

bool finish_detection::isFinished() {
    return this->condition_achieved;
}

void finish_detection::setOutputDir(string outputDir) {
    this->outputDir = outputDir;
}
string finish_detection::getPreviewImageLocation(string stage) {
    return string(this->outputDir).append(stage).append(".jpg");
}
