//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_OBSERVER_H
#define CARMAGEDDON_OBSERVER_H
#include <string>
#include <jansson.h>
#include <sys/stat.h>
#include "opencv2/core/utility.hpp"
#include "camera.h"

class observer {
public:
    observer(Camera *camera) {
        this->camera = camera;
    }
    virtual ~observer() {};
    virtual json_t* getJson(bool full = false) =0;
    virtual int updateWithJson(json_t* root) =0;
    virtual observer* processSnapshot(cv::Mat snapshot) =0;
    virtual void setActive(bool active) =0;

    std::string getType(void) {
        return this->type;
    }
    bool isActive() {
        return this->active;
    }
    bool condition_achieved = false;
    std::string outputDir;
    cv::Rect verifyRoi(cv::Rect roi) {
        cv::Size dimensions = this->camera->getDimensions();
        if (roi.x + roi.width > dimensions.width) {
            if (roi.x > dimensions.width) {
                roi.x = 0;
                roi.width = dimensions.width;
            } else {
                roi.width = dimensions.width - roi.x;
            }
        }
        if (roi.y + roi.height > dimensions.height) {
            if (roi.y > dimensions.height) {
                roi.y = 0;
                roi.height = dimensions.height;
            } else {
                roi.height = dimensions.height - this->roi.y;
            }
        }
        return roi;
    }

    void _mkdir(const char *dir) {
        char tmp[256];
        char *p = NULL;
        size_t len;

        snprintf(tmp, sizeof(tmp),"%s",dir);
        len = strlen(tmp);
        if(tmp[len - 1] == '/')
            tmp[len - 1] = 0;
        for(p = tmp + 1; *p; p++)
            if(*p == '/') {
                *p = 0;
                mkdir(tmp, S_IRWXU);
                *p = '/';
            }
        mkdir(tmp, S_IRWXU);
    }

    void writeImage(std::string imageName, cv::Mat image) {
        _mkdir(this->outputDir.c_str());
        cv::imwrite(std::string(this->outputDir).append(imageName), image);
    }

    void setOutputDir(std::string outputDir) {
        this->outputDir = std::string(outputDir).append(this->type).append("/");
    }

protected:
    bool active = false;
    std::string type;
    cv::Rect roi;
    cv::Mat last_snapshot;
    Camera *camera;
};

#endif //CARMAGEDDON_OBSERVER_H
