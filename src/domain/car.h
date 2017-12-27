//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_CAR_H
#define CARMAGEDDON_CAR_H


#include "engine.h"
#include "steer.h"

enum class car_mode { stopped = 0, manual = 1, autonomous = 2, num_values };

class Car {
public:
    Car();
    int setAngle(int angle);
    int getAngle();
    int setThrottle(int throttle);
    int getThrottle();
    bool getEnabled();
    void setEnabled(bool enable);
    car_mode getMode();
    int setMode(car_mode);

    bool isAutonomous();

private:
    Steer steer;
    Engine engine;
    car_mode mode = car_mode::manual;
    bool enabled = false;
};

#endif //CARMAGEDDON_CAR_H
