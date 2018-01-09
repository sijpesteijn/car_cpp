//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#include "lane_detection.h"
#include <math.h>

using namespace cv;
using namespace std;

lane_detection::lane_detection(Camera* camera):observer(camera) {
    this->type = "lane_detection";
    Size dimensions = camera->getDimensions();
    this->roi = Rect(0, 0, dimensions.width, dimensions.height);
    this->condition_achieved = true;
    this->p = new PIDController(this->kp, this->ki, this->kd);
}

json_t* lane_detection::getJson(bool full) {
    json_t *root = json_object();
    json_object_set_new( root, "type", json_string( this->type.c_str() ) );
    if (full) {
        json_object_set_new(root, "condition_achieved", json_boolean(this->condition_achieved));
        json_object_set_new(root, "selected", json_boolean(this->selected));
        json_object_set_new(root, "error", json_real(this->error));
        json_object_set_new(root, "angle", json_real(this->angle));
    }
    json_object_set_new( root, "pMax", json_real( this->pMax ) );
    json_object_set_new( root, "pMin", json_real( this->pMin ) );
    json_object_set_new( root, "kp", json_real( this->kp ) );
    json_object_set_new( root, "ki", json_real( this->ki ) );
    json_object_set_new( root, "kd", json_real( this->kd ) );
    json_object_set_new( root, "threshold1", json_real( this->threshold1 ) );
    json_object_set_new( root, "threshold2", json_real( this->threshold2 ) );
    json_object_set_new( root, "apertureSize", json_real( this->apertureSize ) );
    json_object_set_new( root, "allLines", json_boolean(this->allLines));
    json_t* roi = json_object();
    json_object_set_new( roi, "x", json_real( this->roi.x ) );
    json_object_set_new( roi, "y", json_real( this->roi.y ) );
    json_object_set_new( roi, "height", json_real( this->roi.height ) );
    json_object_set_new( roi, "width", json_real( this->roi.width ) );
    json_object_set_new( root, "roi", roi);
    return root;
}

int lane_detection::updateWithJson(json_t* root) {
    json_t* roi = json_object_get(root, "roi");
    this->roi.x = static_cast<int>(json_real_value(json_object_get(roi, "x")));
    this->roi.y = static_cast<int>(json_real_value(json_object_get(roi, "y")));
    this->roi.width = static_cast<int>(json_real_value(json_object_get(roi, "width")));
    this->roi.height = static_cast<int>(json_real_value(json_object_get(roi, "height")));

    this->pMax = json_real_value(json_object_get(root, "pMax"));
    this->pMin = json_real_value(json_object_get(root, "pMin"));
    this->kp = json_real_value(json_object_get(root, "kp"));
    this->ki = json_real_value(json_object_get(root, "ki"));
    this->kd = json_real_value(json_object_get(root, "kd"));
    this->threshold1 = json_real_value(json_object_get(root, "threshold1"));
    this->threshold2 = json_real_value(json_object_get(root, "threshold2"));
    this->apertureSize = static_cast<int>(json_real_value(json_object_get(root, "apertureSize")));
    this->allLines = json_boolean_value(json_object_get(root, "allLines"));

    this->p->setOutputLimits(this->pMax, this->pMin);
    this->p->setD(this->kd);
    this->p->setI(this->ki);
    this->p->setP(this->kp);
    this->roi = this->verifyRoi(this->roi);
    return 0;
}

observer* lane_detection::processSnapshot(Mat snapshot) {
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
        list<opencv_line> vertical_left;
        list<opencv_line> vertical_right;
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
            if (degree > 25 && degree < 155) {
                if (p1.y > p2.y) {
                    if (this->allLines) {
                        line(cvt, p1, p2, Scalar(255, 0, 255), 3, CV_AA);
                        circle(cvt, p1, 5, Scalar(255, 0, 0), 5);
                        circle(cvt, p2, 5, Scalar(0, 0, 255), 5);
                    }
                    vertical_left.emplace_back(p1, p2);
                } else {
                    if (this->allLines) {
                        line(cvt, p1, p2, Scalar(0, 255, 255), 3, CV_AA);
                        circle(cvt, p1, 5, Scalar(255, 0, 0), 5);
                        circle(cvt, p2, 5, Scalar(0, 0, 255), 5);
                    }
                    vertical_right.emplace_back(p1, p2);
                }
            }
        }

        double middle = this->camera->getDimensions().width / 2;
        opencv_line *left_average = this->getAverageLine(vertical_left);
        opencv_line *right_average = this->getAverageLine(vertical_right);
        if (left_average != NULL) {
            int deltaX_left = abs(left_average->p2.x - left_average->p1.x);
            int deltaY_left = abs(left_average->p2.y - left_average->p1.y);

            double angle_left = atan2(deltaY_left, deltaX_left) * 180 / M_PI;
            double left_y = 0;
            if (90 - angle_left > 0) {
                left_y = left_average->p1.x / tan((90 - angle_left) * M_PI / 180.0);
            }
            left_average->p1.x = 0;
            left_average->p1.y += left_y;

            double left_x = left_average->p2.y * tan((90 - angle_left) * M_PI / 180.0);
            left_average->p2.x += left_x;
            left_average->p2.y = 0;
            line(cvt, left_average->p1, left_average->p2, Scalar(255, 0, 0), 3, CV_AA);
        }
        if (right_average != NULL) {
            int deltaX_right = abs(right_average->p1.x - right_average->p2.x);
            int deltaY_right = abs(right_average->p1.y - right_average->p2.y);

            double angle_right = atan2(deltaY_right, deltaX_right) * 180 / M_PI;
            double right_y = 0;
            if (90 - angle_right > 0) {
                right_y = (this->camera->getDimensions().width - right_average->p2.x) / tan((90 - angle_right) * M_PI / 180.0);
            }
            right_average->p2.x = this->camera->getDimensions().width;
            right_average->p2.y += right_y;

            double right_x = right_average->p1.y * tan((90 - angle_right) * M_PI / 180.0);
            right_average->p1.x -=right_x;
            right_average->p1.y = 1;
            line(cvt, right_average->p1, right_average->p2, Scalar(0, 255, 0), 3, CV_AA);
        }
        if (left_average != NULL && right_average != NULL) {
            list<opencv_line> average_lines;
            Point p1 = right_average->p1;
            right_average->p1 = right_average->p2;
            right_average->p2 = p1;
            average_lines.push_back(*right_average);
            average_lines.push_back(*left_average);
            opencv_line *average = this->getAverageLine(average_lines);
            this->error = this->p->getOutput(middle, average->p2.x);

            line(cvt, average->p1, average->p2, Scalar(0, 0, 255), 3, CV_AA);
        } else {
            if (left_average) {
                this->error = this->p->getOutput(middle, this->camera->getDimensions().width);
            } else if (right_average) {
                this->error = this->p->getOutput(middle, 0);
            } else {
                cout << "******* NO LINES DETECTED **********" << endl;
            }
        }
        writeImage("cvt.jpg", cvt);
    }
    return this;
}

opencv_line *lane_detection::getAverageLine(std::list<opencv_line> lines) {
    if (lines.size() == 1) {
        return &lines.front();
    }
    unsigned long length = lines.size();
    int x1 = 0, x2 = 0, y1 = 0, y2 = 0;
    for (auto &line : lines) {
        x1 += line.p1.x;
        x2 += line.p2.x;
        y1 += line.p1.y;
        y2 += line.p2.y;
    }
    if (x1 > 0 && x2 > 0 && y1 > 0 && y2 > 0) {
        return new opencv_line(Point(static_cast<int>(x1 / length), static_cast<int>(y1 / length)),
                Point(static_cast<int>(x2 / length), static_cast<int>(y2 / length)));
    }
    return NULL;
}

void lane_detection::setSelected(bool selected) {
    this->selected = selected;
    this->condition_achieved = true;
    struct stat info;
    if (stat(this->outputDir.c_str(), &info) != 0) {
        std::cout << "making " << this->outputDir << std::endl;
        _mkdir(this->outputDir.c_str());
    }
}

void lane_detection::setOutputDir(string outputDir) {
    this->outputDir = outputDir;
}

string lane_detection::getPreviewImageLocation(string stage) {
    return string(this->outputDir).append(stage).append(".jpg");
}

void lane_detection::setRunning(bool running) {
    this->running = running;
}

bool lane_detection::isFinished() {
    return this->condition_achieved;
}