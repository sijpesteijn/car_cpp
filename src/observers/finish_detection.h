//
// Created by Gijs Sijpesteijn on 17/11/2017.
//

#ifndef CARMAGEDDON_FINISH_DETECTION_H
#define CARMAGEDDON_FINISH_DETECTION_H


#include "../domain/observer.h"

class finish_detection: public observer {
public:
    finish_detection(Camera *camera, observer* next_observer);
    json_t* getJson(void);
    int updateWithJson(json_t* root);
    observer* processSnapshot(cv::Mat snapshot);
private:
    Camera* camera;
};


#endif //CARMAGEDDON_FINISH_DETECTION_H
