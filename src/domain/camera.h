//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_CAMERA_H
#define CARMAGEDDON_CAMERA_H

#include <opencv2/opencv.hpp>
#include <pthread.h>
#include <vector>
#include <jansson.h>
#include "../util/settings.h"


class Camera {
public:
    Camera();
    cv::Mat getFrame();
    cv::Size getDimensions();
    void setDimension(int width, int height);
    cv::VideoCapture cap;
    cv::Mat frame;
    pthread_cond_t frame_not_empty = PTHREAD_COND_INITIALIZER;

    int capture_delay = 400;
    int observers_delay = 2000;
    int preview_delay = 6 * 1000;
    double whitebalance_alpha = 1.1;
    int whitebalance_beta = 0;

    void close();

    void fromJson(json_t *pJson);
    json_t* getJson();

private:
    std::vector<cv::Vec2f> detectLines(cv::Mat src);
    settings *sett;

};


#endif //CARMAGEDDON_CAMERA_H
