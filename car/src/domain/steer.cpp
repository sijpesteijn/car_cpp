//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#include "steer.h"
#include "../util/log.h"

#define ONE_DEGREE 8250
#define DUTY_MIDDLE 1480000

Steer::Steer():pwm("P9.22") {
}

int Steer::setAngle(int angle) {
    if (this->pwm.getEnabled() == 1) {
        this->angle = angle;
        if (this->angle < -40) {
            this->angle = -40;
        }
        if (this->angle > 40) {
            this->angle = 40;
        }
        int duty_cycle = DUTY_MIDDLE + (this->angle * ONE_DEGREE);
        this->pwm.setDutyCycle(duty_cycle);
        log::debug(string("Angle set to ").append(to_string(this->angle)));
        return 0;
    }
    return 1;
}

int Steer::getAngle() {
    return this->angle;
}

void Steer::setEnable(bool enable) {
    this->pwm.setEnable(enable);
}
