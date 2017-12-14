//
// Created by Gijs Sijpesteijn on 06/10/2017.
//
#include "camera.h"
#include <syslog.h>
#include <thread>
#include <string>

using namespace cv;
using namespace std;

pthread_mutex_t frame_lock = PTHREAD_MUTEX_INITIALIZER;

void *frameGrabber(void *params) {
    Camera *camera = (Camera *) params;
    while (1) {
        if (pthread_mutex_lock(&frame_lock) != 0) {
            syslog(LOG_ERR, "%s", "Sockethandler: Could not get a lock on the queue");
        }
        camera->cap >> camera->frame;
//        float alpha = 1.0; // 1.0 - 3.0
//        int beta = 0; // 0 - 100;
//        Mat new_image = Mat::zeros(camera->frame.size(), camera->frame.type());
//        for (int y = 0; y < camera->frame.rows; y++) {
//            for (int x = 0; x < camera->frame.cols; x++) {
//                for (int c = 0; c < 3; c++) {
//                    new_image.at<Vec3b>(y, x)[c] =
//                            saturate_cast<uchar>(alpha * (camera->frame.at<Vec3b>(y, x)[c]) + beta);
//                }
//            }
//        }
//        camera->frame = new_image;
        pthread_cond_signal(&camera->frame_not_empty);
        if (pthread_mutex_unlock(&frame_lock) != 0) {
            syslog(LOG_ERR, "%s", "Sockethandler: Could not unlock the queue");
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(camera->capture_delay));
    }
    return NULL;
}

Camera::Camera() : cap(0) {
    this->sett = new settings("../src/camera.json");
    this->fromJson(sett->getSettings());
    this->cap.release();
    this->cap.open(1);
    this->cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    this->cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    pthread_t grabber;
    pthread_create(&grabber, NULL, frameGrabber, this);
}

Mat Camera::getFrame() {
    return this->frame;
}

vector<Vec2f> Camera::detectLines(Mat img) {
    vector<Vec2f> lines;
    HoughLinesP(img, lines, 1, CV_PI / 180, 50, 50, 10);
    return lines;
}

Size Camera::getDimensions() {
    return Size(this->cap.get(CV_CAP_PROP_FRAME_WIDTH), this->cap.get(CV_CAP_PROP_FRAME_HEIGHT));
}

void Camera::setDimension(int width, int height) {
    this->cap.set(CV_CAP_PROP_FRAME_WIDTH, width);
    this->cap.set(CV_CAP_PROP_FRAME_HEIGHT, height);
}

void Camera::close() {
    this->cap.release();
    this->sett->save(this->getJson());
}

void Camera::fromJson(json_t *pJson) {
    json_t *resolutionJson = json_object_get(pJson,"dimension");
    int width = json_number_value(json_object_get(resolutionJson, "width"));
    int height = json_number_value(json_object_get(resolutionJson, "height"));
    this->setDimension(width, height);
    this->capture_delay = json_number_value(json_object_get(pJson, "captureDelay"));
    this->observers_delay = json_number_value(json_object_get(pJson, "observersDelay"));
    this->preview_delay = json_number_value(json_object_get(pJson, "previewDelay"));
    json_t *whiteBalance = json_object_get(pJson, "whiteBalance");
    this->whitebalance_alpha = json_real_value(json_object_get(whiteBalance, "alpha"));
    this->whitebalance_beta = json_number_value(json_object_get(whiteBalance, "beta"));
    this->sett->save(this->getJson());
}

json_t* Camera::getJson() {
    json_t *root = json_object();
    json_t* dimension = json_object();
    json_object_set_new( dimension, "name", json_string( string(to_string(this->getDimensions().width) + "x" + to_string(this->getDimensions().height)).c_str() ) );
    json_object_set_new( dimension, "width", json_integer( this->getDimensions().width ) );
    json_object_set_new( dimension, "height", json_integer( this->getDimensions().height ) );
    json_object_set_new( root, "dimension", dimension);

    json_object_set_new( root, "captureDelay", json_integer( this->capture_delay ) );
    json_object_set_new( root, "observersDelay", json_integer( this->observers_delay ) );
    json_object_set_new( root, "previewDelay", json_integer( this->preview_delay ) );
    json_t *whiteBalance = json_object();
    json_object_set_new( whiteBalance, "alpha", json_real(this->whitebalance_alpha ) );
    json_object_set_new( whiteBalance, "beta", json_integer( this->whitebalance_beta) );
    json_object_set_new( root, "whiteBalance", whiteBalance );

    return root;
}