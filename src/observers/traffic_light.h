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
    json_t* getJson(bool full = false);
    int updateWithJson(json_t* root);
    observer* processSnapshot(cv::Mat snapshot);
    void setActive(bool active);
private:
    int pixel_difference = 70;
    int current_pixel_difference = 0;
};

#endif //CARMAGEDDON_TRAFFIC_LIGHT_H
