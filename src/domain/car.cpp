//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#include "car.h"
#include "../util/log.h"


Car::Car() {
    this->enabled = 0;
    this->mode = car_mode::stopped;
}

int Car::setMode(car_mode mode) {
    if (this->enabled == 1) {
        this->mode = mode;
        log::debug(string("Mode set to: ").append(to_string(static_cast<std::underlying_type<car_mode>::type>(mode))));
        return 0;
    }
    log::debug(string("Car not enabled"));
    return -1;
}

int Car::setAngle(int angle) {
    if (this->enabled == 1) {
        this->steer.setAngle(angle);
        log::debug(string("Angle set to: ").append(to_string(angle)));
        return 0;
    }
    log::debug(string("Car not enabled."));
    return -1;
}

int Car::setThrottle(int throttle) {
    if (this->enabled == 1) {
        this->engine.setThrottle(throttle);
        log::debug(string("Throttle set to: ").append(to_string(throttle)));
        return 0;
    }
    log::debug(string("Car not enabled."));
    return -1;
}

void Car::setEnabled(bool enabled) {
    log::debug(string("Car enabled set to: ").append(enabled ? "true" : "false"));
    this->enabled = enabled;
    if (this->enabled == false) {
        this->mode = car_mode::stopped;
    }
    this->steer.setEnable(this->enabled);
    this->engine.setEnable(this->enabled);
}

car_mode Car::getMode() {
    return this->mode;
}

int Car::getAngle() {
    return this->steer.getAngle();
}

int Car::getThrottle() {
    return this->engine.getThrottle();
}

bool Car::getEnabled() {
    return this->enabled;
}

bool Car::isAutonomous() {
    return this->mode == car_mode::autonomous;
}
