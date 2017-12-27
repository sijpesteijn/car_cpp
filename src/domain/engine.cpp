//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#include "engine.h"
#include "../util/log.h"

#define DUTY_MIDDLE 1500000
#define SPEED_STEP 15000

Engine::Engine():pwm("P9.21") {
}

int Engine::setThrottle(int throttle) {
    if (this->pwm.getEnabled() == 1) {
        this->throttle = throttle;
        if (this->throttle < -100) {
            this->throttle = -100;
        }
        if (this->throttle >100) {
            this->throttle = 100;
        }
        int duty_cycle = DUTY_MIDDLE + (this->throttle * SPEED_STEP);
        this->pwm.setDutyCycle(duty_cycle);
        log::debug(string("Throttle set to ").append(to_string(this->throttle)));
        return 0;
    } else {
        return -1;
    }
}

int Engine::getThrottle() {
    return this->throttle;
}

void Engine::setEnable(bool enable) {
    this->pwm.setEnable(enable);
}