//
// Created by Gijs Sijpesteijn on 17/11/2017.
//

#ifndef CARMAGEDDON_FINISH_DETECTION_H
#define CARMAGEDDON_FINISH_DETECTION_H


#include "observer.h"

class finish_detection: public observer {
public:
    finish_detection(Camera *camera);
    json_t* getJson(bool full = false);
    int updateWithJson(json_t* root);
    observer* processSnapshot(cv::Mat snapshot);
    void setSelected(bool selected);
private:
};


#endif //CARMAGEDDON_FINISH_DETECTION_H
