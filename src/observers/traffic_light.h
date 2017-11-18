//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_TRAFFIC_LIGHT_H
#define CARMAGEDDON_TRAFFIC_LIGHT_H

#include "../domain/observer.h"
#include "../domain/camera.h"
#include <jansson.h>

class traffic_light:public observer {
public:
    traffic_light(Camera* camera);
    std::string getJson(void);
    int updateWithJson(json_t* root);
    observer* processSnapshot(cv::Mat snapshot);
    cv::Rect verifyRoi();
private:
    Camera* camera;
    int count = 0;
    int pixel_difference = 70;
    int current_pixel_difference = 0;
};

#endif //CARMAGEDDON_TRAFFIC_LIGHT_H
