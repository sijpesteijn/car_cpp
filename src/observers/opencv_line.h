//
// Created by Gijs Sijpesteijn on 18/12/2017.
//

#ifndef CARMAGEDDON_LINE_H
#define CARMAGEDDON_LINE_H

#include "opencv2/core/utility.hpp"

class opencv_line {
public:
    opencv_line(cv::Point p1, cv::Point p2);
    cv::Point p1;
    cv::Point p2;
};


#endif //CARMAGEDDON_LINE_H
