//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_LANE_DETECTION_H
#define CARMAGEDDON_LANE_DETECTION_H

#include "observer.h"
#include "../domain/camera.h"
#include "Line.h"
#include "../util/pid.h"
#include <jansson.h>

class lane_detection:public observer {
public:
    explicit lane_detection(Camera* camera);
    json_t* getJson(bool full = false) override;
    int updateWithJson(json_t* root) override;
    observer* processSnapshot(cv::Mat snapshot) override;
    cv::Rect verifyRoi();
    void setSelected(bool selected) override;
    bool isFinished() override;
    void setRunning(bool running) override;
    std::string getPreviewImageLocation(std::string stage) override;
private:
    double threshold1 = 50;
    double threshold2 = 200;
    int apertureSize = 3;
    double kc = 1.0;
    double ki = 1.0;
    double kd = 1.0;
    double pMax = 100.0;
    double pMin = -100.0;
    double error = 0.0;
    double angle = 0.0;

    Line *getAverageLine(std::list<Line> lines);
    void setOutputDir(std::string outputDir) override;

protected:
    pid *p;
};

#endif //CARMAGEDDON_LANE_DETECTION_H
