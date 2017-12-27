//
// Created by Gijs Sijpesteijn on 22/12/2017.
//

#ifndef CARMAGEDDON_DUMMY_H
#define CARMAGEDDON_DUMMY_H


#include "../domain/observer.h"

class dummy: public observer {
public:
    dummy(Camera *camera);
    json_t* getJson(bool full = false);
    int updateWithJson(json_t* root);
    observer* processSnapshot(cv::Mat snapshot);
    void setActive(bool active);
};


#endif //CARMAGEDDON_DUMMY_H
