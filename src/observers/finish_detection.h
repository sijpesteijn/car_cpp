//
// Created by Gijs Sijpesteijn on 17/11/2017.
//

#ifndef CARMAGEDDON_FINISH_DETECTION_H
#define CARMAGEDDON_FINISH_DETECTION_H


#include "observer.h"

class finish_detection: public observer {
public:
    finish_detection(Camera *camera);
    json_t* getJson(bool full = false) override;
    int updateWithJson(json_t* root) override;
    observer* processSnapshot(cv::Mat snapshot) override;
    void setSelected(bool selected) override;
    void setRunning(bool running) override;
    bool isFinished() override;
    std::string getPreviewImageLocation(std::string stage) override;
private:
    double threshold1 = 50;
    double threshold2 = 200;
    int apertureSize = 3;
    void setOutputDir(std::string outputDir) override;
};


#endif //CARMAGEDDON_FINISH_DETECTION_H
