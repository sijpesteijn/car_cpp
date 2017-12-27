//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_LANE_DETECTION_H
#define CARMAGEDDON_LANE_DETECTION_H

#include "../domain/observer.h"
#include "../domain/camera.h"
#include "Line.h"
#include <jansson.h>

class lane_detection:public observer {
public:
    lane_detection(Camera* camera);
    json_t* getJson(bool full = false);
    int updateWithJson(json_t* root);
    observer* processSnapshot(cv::Mat snapshot);
    cv::Rect verifyRoi();
    void setActive(bool active);
private:
    double threshold1 = 50;
    double threshold2 = 200;
    int apertureSize = 3;

    Line getAverageLine(std::list<Line> lines);
};

#endif //CARMAGEDDON_LANE_DETECTION_H
