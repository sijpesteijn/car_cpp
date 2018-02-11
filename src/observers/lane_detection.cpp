//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#include "lane_detection.h"
#include "../util/log.h"
#include <math.h>

using namespace cv;
using namespace std;

lane_detection::lane_detection(Camera* camera, Car* car):observer(camera, car) {
    this->type = "lane_detection";
    Size dimensions = camera->getDimensions();
    this->roi = Rect(0, 0, dimensions.width, dimensions.height);
    this->condition_achieved = true;
    this->pid = new PIDController(this->kp, this->ki, this->kd);
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
    json_object_set_new( root, "warp", json_boolean( this->warp ) );
    json_object_set_new( root, "previewType", json_string(this->previewType));
    json_object_set_new( root, "showFoundLines", json_boolean(this->showFoundLines));
    json_object_set_new( root, "showAverageLRLines", json_boolean(this->showAverageLRLines));
    json_object_set_new( root, "showAverageLine", json_boolean(this->showAverageLine));
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
    this->warp = json_boolean_value(json_object_get(root, "warp"));
    this->previewType = json_string_value(json_object_get(root, "previewType"));
    this->showFoundLines = json_boolean_value(json_object_get(root, "showFoundLines"));
    this->showAverageLRLines = json_boolean_value(json_object_get(root, "showAverageLRLines"));
    this->showAverageLine = json_boolean_value(json_object_get(root, "showAverageLine"));

    this->pid->setOutputLimits(this->pMax, this->pMin);
    this->pid->setD(this->kd);
    this->pid->setI(this->ki);
    this->pid->setP(this->kp);
    this->roi = this->verifyRoi(this->roi);
    return 0;
}

observer* lane_detection::processSnapshot(Mat snapshot) {
    if (snapshot.total() > 0 && this->last_snapshot.total() > 0)
    {
        Mat blurred, gaussian, canny, cvt, matrix, warped;
        Point2f inputQuad[4];
        Point2f outputQuad[4];

        this->roi = this->verifyRoi(this->roi);
        Mat roiSnapshot = snapshot(this->roi);
        if (this->warp) {
            matrix = Mat::zeros(roiSnapshot.rows, roiSnapshot.cols, roiSnapshot.type());
            inputQuad[0] = Point2f(0, 0);
            inputQuad[1] = Point2f(matrix.cols, 0);
            inputQuad[2] = Point2f(matrix.cols, matrix.rows);
            inputQuad[3] = Point2f(0, matrix.rows);
            outputQuad[0] = Point2f(0, 0);
            outputQuad[1] = Point2f(roiSnapshot.cols, 0);
            outputQuad[2] = Point2f(roiSnapshot.cols - this->warp, roiSnapshot.rows);
            outputQuad[3] = Point2f(this->warp, roiSnapshot.rows);

            matrix = getPerspectiveTransform(inputQuad, outputQuad);

            warpPerspective(roiSnapshot, warped, matrix, roiSnapshot.size());
            writeImage("warped.jpg", warped);
        } else {
            warped = roiSnapshot;
        }
        GaussianBlur(warped, gaussian, Size(11, 11), 0);
        writeImage("gaussian.jpg", gaussian);

        Canny(gaussian, canny, this->threshold1, this->threshold2, this->apertureSize);
        writeImage("canny.jpg", canny.clone());
        cvtColor(canny, cvt, CV_GRAY2BGR);

        vector<Vec4i> lines;
        HoughLinesP(canny, lines, 1, CV_PI / 180, 50, 50, 10);
        if (lines.size() > 0) {
            list<opencv_line> vertical_left;
            list<opencv_line> vertical_right;
            this->getVerticalLines(lines, vertical_left, vertical_right, cvt);

            opencv_line *average_left = this->getAverageLine(vertical_left);
            this->drawLeftAverage(average_left, cvt);
            opencv_line *average_right = this->getAverageLine(vertical_right);
            this->drawRightAverage(average_right, cvt);
            this->drawAverage(average_left, average_right, cvt);
        } else {
            log::debug("******* NO LINES DETECTED **********");
        }
        writeImage("cvt.jpg", cvt.clone());
    } else {
        this->last_snapshot = snapshot;
    }
    return this;
}

opencv_line *lane_detection::getAverageLine(std::list<opencv_line> lines) {
    if (lines.size() == 0) {
        return NULL;
    }
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

void lane_detection::getVerticalLines(vector<cv::Vec4i> lines, list<opencv_line> vertical_left, list <opencv_line> vertical_right, Mat cvt) {
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
                if (this->showFoundLines) {
                    line(cvt, p1, p2, Scalar(255, 0, 255), 3, CV_AA);
                    circle(cvt, p1, 5, Scalar(255, 0, 0), 5);
                    circle(cvt, p2, 5, Scalar(0, 0, 255), 5);
                }
                vertical_left.emplace_back(p1, p2);
            } else {
                if (this->showFoundLines) {
                    line(cvt, p1, p2, Scalar(0, 255, 255), 3, CV_AA);
                    circle(cvt, p1, 5, Scalar(255, 0, 0), 5);
                    circle(cvt, p2, 5, Scalar(0, 0, 255), 5);
                }
                vertical_right.emplace_back(p1, p2);
            }
        }
    }
}

void lane_detection::drawLeftAverage(opencv_line *average_left, cv::Mat cvt) {
    if (this->showAverageLRLines == true && average_left != NULL) {
        int deltaX_left = abs(average_left->p2.x - average_left->p1.x);
        int deltaY_left = abs(average_left->p2.y - average_left->p1.y);

        double angle_left = atan2(deltaY_left, deltaX_left) * 180 / M_PI;
        double left_y = 0;
        if (90 - angle_left > 0) {
            left_y = average_left->p1.x / tan((90 - angle_left) * M_PI / 180.0);
        }
        average_left->p1.x = 0;
        average_left->p1.y += left_y;

        double left_x = average_left->p2.y * tan((90 - angle_left) * M_PI / 180.0);
        average_left->p2.x += left_x;
        average_left->p2.y = 0;
        line(cvt, average_left->p1, average_left->p2, Scalar(255, 0, 0), 3, CV_AA);
    }
}

void lane_detection::drawRightAverage(opencv_line *average_right, cv::Mat cvt) {
    if (this->showAverageLRLines == true && average_right != NULL) {
        int deltaX_right = abs(average_right->p1.x - average_right->p2.x);
        int deltaY_right = abs(average_right->p1.y - average_right->p2.y);

        double angle_right = atan2(deltaY_right, deltaX_right) * 180 / M_PI;
        double right_y = 0;
        if (90 - angle_right > 0) {
            right_y = (this->camera->getDimensions().width - average_right->p2.x) / tan((90 - angle_right) * M_PI / 180.0);
        }
        average_right->p2.x = this->camera->getDimensions().width;
        average_right->p2.y += right_y;

        double right_x = average_right->p1.y * tan((90 - angle_right) * M_PI / 180.0);
        average_right->p1.x -=right_x;
        average_right->p1.y = 1;
        line(cvt, average_right->p1, average_right->p2, Scalar(0, 255, 0), 3, CV_AA);
    }
}

void lane_detection::drawAverage(opencv_line *average_left, opencv_line *average_right, cv::Mat cvt) {
    double middle = this->camera->getDimensions().width / 2;
    if (this->showAverageLine == true && average_left != NULL && average_right != NULL) {
        list<opencv_line> average_lines;
        Point p1 = average_right->p1;
        average_right->p1 = average_right->p2;
        average_right->p2 = p1;
        average_lines.push_back(*average_right);
        average_lines.push_back(*average_left);
        opencv_line *average = this->getAverageLine(average_lines);
        if (average) {
            this->error = this->pid->getOutput(middle, average->p2.x);
            if (this->running) {
                this->car->setAngle(40);
            }
            line(cvt, average->p1, average->p2, Scalar(0, 0, 255), 3, CV_AA);
        } else {
            log::debug("Could not find average");
        }
    } else {
        if (average_left != NULL) {
            this->error = this->pid->getOutput(middle, this->camera->getDimensions().width);
        } else if (average_right != NULL) {
            this->error = this->pid->getOutput(middle, 0);
        } else {
            log::debug("******* NO AVERAGE LINES DETECTED **********");
        }
    }
}
