//
// Created by Gijs Sijpesteijn on 06/10/2017.
//
#include "camera.h"
#include "../util/log.h"
#include <thread>

using namespace cv;
using namespace std;

pthread_mutex_t frame_lock = PTHREAD_MUTEX_INITIALIZER;

void *frameGrabber(void *params) {
    Camera *camera = (Camera *) params;
    bool run = true;
    while (run) {
        if (pthread_mutex_lock(&frame_lock) != 0) {
            log::debug(string("Sockethandler: Could not get a lock on the queue"));
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
            log::debug(string("Sockethandler: Could not unlock the queue"));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(camera->capture_delay));
    }
    return NULL;
}

Camera::Camera() : cap("http://192.168.88.16:8090/?action=stream") {
    this->sett = new settings("./resources/camera.json");
    this->fromJson(sett->getSettings());
//    if (this->cap.open(1) == false) {
//        this->cap.open("http://192.168.88.16:8090/?action=stream?dummy=param.mjpg");
//    }
    this->cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    this->cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    log::debug(string("Width ").append(to_string(this->cap.get(CV_CAP_PROP_FRAME_WIDTH))));
    log::debug(string("Height ").append(to_string(this->cap.get(CV_CAP_PROP_FRAME_HEIGHT))));
    log::debug(string("Brightness ").append(to_string(this->cap.get(CV_CAP_PROP_BRIGHTNESS))));
    log::debug(string("Contrast ").append(to_string(this->cap.get(CV_CAP_PROP_CONTRAST))));
    log::debug(string("Hue ").append(to_string(this->cap.get(CV_CAP_PROP_HUE))));
    log::debug(string("FPS ").append(to_string(this->cap.get(CV_CAP_PROP_FPS))));
    log::debug(string("Saturation ").append(to_string(this->cap.get(CV_CAP_PROP_SATURATION))));
    log::debug(string("Gain ").append(to_string(this->cap.get(CV_CAP_PROP_GAIN))));
    log::debug(string("Convert rgb ").append(to_string(this->cap.get(CAP_PROP_CONVERT_RGB))));
    log::debug(string("White balance blue ").append(to_string(this->cap.get(CAP_PROP_WHITE_BALANCE_BLUE_U))));
    log::debug(string("White balance red ").append(to_string(this->cap.get(CAP_PROP_WHITE_BALANCE_RED_V))));

    pthread_t grabber;
    pthread_create(&grabber, NULL, frameGrabber, this);
}

Mat Camera::getFrame() {
    return this->frame;
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
    int brightness = json_number_value(json_object_get(pJson, "brightness"));
    if (brightness > 0) {
        this->cap.set(CV_CAP_PROP_BRIGHTNESS, brightness);
    }
    int contrast = json_number_value(json_object_get(pJson, "contrast"));
    if (contrast > 0) {
        this->cap.set(CV_CAP_PROP_CONTRAST, contrast);
    }
    int saturation = json_number_value(json_object_get(pJson, "saturation"));
    if (saturation > 0) {
        this->cap.set(CV_CAP_PROP_SATURATION, saturation);
    }
    int hue = json_number_value(json_object_get(pJson, "hue"));
    if (hue > 0) {
        this->cap.set(CV_CAP_PROP_HUE, hue);
    }
    int gain = json_number_value(json_object_get(pJson, "gain"));
    if (gain > 0) {
        this->cap.set(CV_CAP_PROP_GAIN, gain);
    }
    int fps = json_number_value(json_object_get(pJson, "fps"));
    if (fps > 0) {
        this->cap.set(CV_CAP_PROP_FPS, fps);
    }
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
    Size dimensions = this->getDimensions();
    json_object_set_new( dimension, "name", json_string( string(to_string(dimensions.width) + "x" + to_string(dimensions.height)).c_str() ) );
    json_object_set_new( dimension, "width", json_integer( dimensions.width ) );
    json_object_set_new( dimension, "height", json_integer( dimensions.height ) );
    json_object_set_new( root, "dimension", dimension);

    json_object_set_new( root, "captureDelay", json_integer( this->capture_delay ) );
    json_object_set_new( root, "observersDelay", json_integer( this->observers_delay ) );
    json_object_set_new( root, "previewDelay", json_integer( this->preview_delay ) );
    json_t *whiteBalance = json_object();
    json_object_set_new( whiteBalance, "alpha", json_real(this->whitebalance_alpha ) );
    json_object_set_new( whiteBalance, "beta", json_integer( this->whitebalance_beta) );
    json_object_set_new( root, "whiteBalance", whiteBalance );
    json_object_set_new( root, "brightness", json_integer(this->cap.get(CV_CAP_PROP_BRIGHTNESS)));
    json_object_set_new( root, "contrast", json_integer(this->cap.get(CV_CAP_PROP_CONTRAST)));
    json_object_set_new( root, "saturation", json_integer(this->cap.get(CV_CAP_PROP_SATURATION)));
    json_object_set_new( root, "hue", json_integer(this->cap.get(CV_CAP_PROP_HUE)));
    json_object_set_new( root, "gain", json_integer(this->cap.get(CV_CAP_PROP_GAIN)));
    json_object_set_new( root, "fps", json_integer(this->cap.get(CV_CAP_PROP_FPS)));

    return root;
}