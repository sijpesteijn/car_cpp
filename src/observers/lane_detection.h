//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_LANE_DETECTION_H
#define CARMAGEDDON_LANE_DETECTION_H

#include "observer.h"
#include "../domain/camera.h"
#include "Line.h"
#include <jansson.h>

class lane_detection:public observer {
public:
    explicit lane_detection(Camera* camera);
    json_t* getJson(bool full = false) override;
    int updateWithJson(json_t* root) override;
    observer* processSnapshot(cv::Mat snapshot) override;
    cv::Rect verifyRoi();
    void setSelected(bool selected) override;
    std::string getPreviewImageLocation(std::string stage) override;
private:
    double threshold1 = 50;
    double threshold2 = 200;
    int apertureSize = 3;

    Line getAverageLine(std::list<Line> lines);
    void setOutputDir(std::string outputDir) override;

};

#endif //CARMAGEDDON_LANE_DETECTION_H
