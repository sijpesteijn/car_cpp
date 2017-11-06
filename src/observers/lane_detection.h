//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_LANE_DETECTION_H
#define CARMAGEDDON_LANE_DETECTION_H

#include "../domain/observer.h"
#include "../domain/camera.h"
#include <jansson.h>
using namespace cv;
using namespace std;

class lane_detection:public observer {
public:
    lane_detection(Camera* camera);
    string getJson(void);
    int updateWithJson(json_t* root);
    observer* processSnapshot(Mat snapshot);
    Rect verifyRoi();
private:
    Camera* camera;
};

#endif //CARMAGEDDON_LANE_DETECTION_H
