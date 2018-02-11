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
        return 0;
    }
    log::debug(string("Car not enabled."));
    return -1;
}

int Car::setThrottle(int throttle) {
    if (this->enabled == 1) {
        this->engine.setThrottle(throttle);
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

json_t* Car::getJson() {
    json_t *root = json_object();
    json_object_set_new( root, "mode", json_string(to_string(static_cast<std::underlying_type<car_mode>::type>(this->mode)).c_str()));
    json_object_set_new(root, "enabled", json_boolean(this->enabled));
    json_object_set_new(root, "angle", json_integer(this->steer.getAngle()));
    json_object_set_new(root, "throttle", json_integer(this->engine.getThrottle()));
    return root;
}

void Car::fromJson(json_t *root) {
    this->setMode(static_cast<car_mode>(json_integer_value(json_object_get(root, "mode"))));
    this->setAngle(json_integer_value(json_object_get(root, "angle")));
    this->setThrottle(json_integer_value(json_object_get(root, "throttle")));
}