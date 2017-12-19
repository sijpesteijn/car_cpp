//
// Created by Gijs Sijpesteijn on 18/12/2017.
//

#include "Line.h"

using namespace cv;

Line::Line(cv::Point p1, cv::Point p2) {
    this->p1 = p1;
    this->p2 = p2;
}