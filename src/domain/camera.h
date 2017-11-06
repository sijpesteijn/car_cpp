//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_CAMERA_H
#define CARMAGEDDON_CAMERA_H

#include <opencv2/opencv.hpp>
#include <pthread.h>
#include <vector>

using namespace cv;
using namespace std;

class Camera {
public:
    Camera();
    Mat getFrame();
    Size getDimensions();
    void setDimension(int width, int height);
    VideoCapture cap;
    Mat frame;
    pthread_cond_t frame_not_empty = PTHREAD_COND_INITIALIZER;

    int capture_delay = 400;
    int observers_delay = 2000;
    int preview_delay = 6 * 1000;
    double whitebalance_alpha = 1.1;
    int whitebalance_beta = 0;

private:
    vector<Vec2f> detectLines(Mat src);
};


#endif //CARMAGEDDON_CAMERA_H
