//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_OBSERVER_H
#define CARMAGEDDON_OBSERVER_H
#include <string>
#include <jansson.h>
#include "opencv2/core/utility.hpp"
#include "camera.h"

class observer {
public:
    virtual ~observer() {};
    virtual std::string getJson(void) =0;
    virtual int updateWithJson(json_t* root) =0;
    virtual observer* processSnapshot(cv::Mat snapshot) =0;
    observer* nextObserver;

    const char* getType(void) {
        return this->type;
    }
    int isActive() {
        return this->active;
    }
    void setActive(int active) {
        this->active = active;
    }
    int getOrder() {
        return this->order;
    }
    void setOrder(int order) {
        this->order = order;
    }
    int condition_achieved = 0;
protected:
    int active = 0;
    int order = 0;
    const char *type;
    cv::Rect roi;
    cv::Mat last_snapshot;
};

#endif //CARMAGEDDON_OBSERVER_H
