//
// Created by Gijs Sijpesteijn on 06/10/2017.
//
#include "camera.h"
#include "../util/log.h"
#include <thread>
#include <curl/curl.h>

using namespace cv;
using namespace std;

size_t write_data(char *ptr, size_t size, size_t nmemb, void *userdata) {
    std::ostringstream *stream = (std::ostringstream*)userdata;
    size_t count = size * nmemb;
    stream->write(ptr, count);
    return count;
}

Mat Camera::getFrame() {
    if (local) {
        Mat frame;
        this->cap >> frame;
        return frame;
    } else {
        CURL *curl;
        CURLcode res;
        std::ostringstream stream;
        curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.88.10:8090/?action=snapshot"); //the img url
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data); // pass the writefunction
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &stream); // pass the stream ptr when the writefunction is called
        res = curl_easy_perform(curl); // start curl
        std::string output = stream.str(); // convert the stream into a string
        curl_easy_cleanup(curl); // cleanup
        std::vector<char> data = std::vector<char>(output.begin(), output.end()); //convert string into a vector
        cv::Mat data_mat = cv::Mat(data); // create the cv::Mat datatype from the vector
        cv::Mat image = cv::imdecode(data_mat, 1); //read an image from memory buffer
        return image;
    }
}

Camera::Camera(bool local) {
    this->local = local;
    if (this->local) {
        this->cap = VideoCapture(0);
    }
    this->sett = new settings("./resources/camera.json");
    this->fromJson(sett->getSettings());
}

Size Camera::getDimensions() {
    return Size(640,360);
}

void Camera::close() {
    this->sett->save(this->getJson());
}

void Camera::fromJson(json_t *pJson) {
//    this->capture_delay = json_number_value(json_object_get(pJson, "captureDelay"));
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

//    json_object_set_new( root, "captureDelay", json_integer( this->capture_delay ) );
    json_object_set_new( root, "observersDelay", json_integer( this->observers_delay ) );
    json_object_set_new( root, "previewDelay", json_integer( this->preview_delay ) );
    json_t *whiteBalance = json_object();
    json_object_set_new( whiteBalance, "alpha", json_real(this->whitebalance_alpha ) );
    json_object_set_new( whiteBalance, "beta", json_integer( this->whitebalance_beta) );
    json_object_set_new( root, "whiteBalance", whiteBalance );

    return root;
}