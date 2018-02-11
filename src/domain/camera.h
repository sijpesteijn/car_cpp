//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_CAMERA_H
#define CARMAGEDDON_CAMERA_H

#include <opencv2/opencv.hpp>
#include <pthread.h>
#include <vector>
#include <string>
#include <jansson.h>
#include "../util/settings.h"


class Camera {
public:
    Camera(bool local);
    cv::Size getDimensions();

//    int capture_delay = 400;
    int observers_delay = 2000;
    int preview_delay = 6 * 1000;
    double whitebalance_alpha = 1.1;
    int whitebalance_beta = 0;

    void close();

    void fromJson(json_t *pJson);
    json_t* getJson();
    cv::Mat getFrame();
private:
    settings *sett;
    bool local;
    cv::VideoCapture cap;
};


#endif //CARMAGEDDON_CAMERA_H
