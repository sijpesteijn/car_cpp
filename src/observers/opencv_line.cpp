//
// Created by Gijs Sijpesteijn on 18/12/2017.
//

#include "opencv_line.h"

using namespace cv;

opencv_line::opencv_line(cv::Point p1, cv::Point p2) {
    this->p1 = p1;
    this->p2 = p2;
}