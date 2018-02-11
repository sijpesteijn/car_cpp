//
// Created by Gijs Sijpesteijn on 18/12/2017.
//

#include "opencv_line.h"

using namespace cv;

opencv_line::opencv_line(cv::Point2f p1, cv::Point2f p2) {
    this->p1 = p1;
    this->p2 = p2;
}