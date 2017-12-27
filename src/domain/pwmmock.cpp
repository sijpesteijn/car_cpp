//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#import "pwm.h"
#include "../util/log.h"

PWM::PWM(string pwmNr) {
    this->pwmNr = pwmNr;
}

PWM::PWM(string pwmNr, int period, int duty_cycle) {
    this->pwmNr = pwmNr;
    this->setPeriod(period);
    this->setDutyCycle(duty_cycle);
}

void PWM::setPeriod(int period) {
    this->period = period;
    log::debug(string("Period set to ").append(to_string(this->period)));
}

void PWM::setDutyCycle(int duty_cycle) {
    this->duty_cycle = duty_cycle;
    log::debug(string("Duty cycle set to ").append(to_string(this->duty_cycle)));
}

void PWM::setEnable(bool enable) {
    this->enable = enable;
    log::debug(string("Enable set to ").append(enable ? "true" : "false"));
}

bool PWM::getEnabled() {
    return this->enable;
}
