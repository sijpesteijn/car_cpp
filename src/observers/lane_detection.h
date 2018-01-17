//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_LANE_DETECTION_H
#define CARMAGEDDON_LANE_DETECTION_H

#include "observer.h"
#include "../domain/camera.h"
#include "opencv_line.h"
#include "../util/pid_controller.h"
#include "../domain/car.h"
#include <jansson.h>

class lane_detection:public observer {
public:
    explicit lane_detection(Camera* camera, Car* car);
    json_t* getJson(bool full = false) override;
    int updateWithJson(json_t* root) override;
    observer* processSnapshot(cv::Mat snapshot) override;
    void setSelected(bool selected) override;
    bool isFinished() override;
    void setRunning(bool running) override;
    std::string getPreviewImageLocation(std::string stage) override;
private:
    double threshold1 = 50;
    double threshold2 = 200;
    int apertureSize = 3;
    double kp = 1.0;
    double ki = 1.0;
    double kd = 1.0;
    double pMax = 100.0;
    double pMin = -100.0;
    double error = 0.0;
    double angle = 0.0;
    double warp = 0.0;
    bool allLines = false;
    const char* previewType = "cvt";

    opencv_line *getAverageLine(std::list<opencv_line> lines);
    void setOutputDir(std::string outputDir) override;

protected:
    PIDController *p;
};

#endif //CARMAGEDDON_LANE_DETECTION_H
