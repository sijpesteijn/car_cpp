//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_OBSERVER_H
#define CARMAGEDDON_OBSERVER_H
#include <string>
#include <jansson.h>
#include <sys/stat.h>
#include "opencv2/core/utility.hpp"
#include "../domain/camera.h"
#include "../domain/car.h"

class observer {
public:
    observer(Camera *camera, Car* car) {
        this->camera = camera;
        this->car = car;
    }
    virtual ~observer() {};
    virtual json_t* getJson(bool full = false) =0;
    virtual int updateWithJson(json_t* root) =0;
    virtual observer* processSnapshot(cv::Mat snapshot) =0;
    virtual void setSelected(bool selected) =0;
    virtual void setRunning(bool running) =0;
    virtual bool isFinished() =0;
    bool isRunning() {
        return this->running;
    }

    void setActive(bool active) {
        this->active = active;
    }

    bool isActive() {
        return this->active;
    }

    std::string getType(void) {
        return this->type;
    }
    bool isSelected() {
        return this->selected;
    }
    bool condition_achieved = false;
    std::string outputDir;
    cv::Rect verifyRoi(cv::Rect roi) {
        cv::Size dimensions = this->camera->getDimensions();
        if (roi.x < 0) {
            roi.x = 0;
        }
        if (roi.y < 0) {
            roi.y = 0;
        }
        if (roi.width < 0) {
            roi.width = 0;
        }
        if (roi.height < 0) {
            roi.height = 0;
        }
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
//        std::cout << this->outputDir << std::endl;
        cv::imwrite(std::string(this->outputDir).append(imageName), image);
    }

    virtual void setOutputDir(std::string outputDir) {
        this->outputDir = outputDir;
    }

    virtual std::string getPreviewImageLocation(const std::string stage) {
        return std::string();
    }

protected:
    bool selected = false;
    bool active = false;
    bool running = false;
    std::string type;
    cv::Rect roi;
    cv::Mat last_snapshot;
    Camera *camera;
    Car *car;
};

#endif //CARMAGEDDON_OBSERVER_H
