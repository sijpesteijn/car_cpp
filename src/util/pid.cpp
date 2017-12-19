//
// Created by Gijs Sijpesteijn on 18/12/2017.
//

#include "pid.h"
#include <iostream>
#include <cmath>

pid::pid(double dt, double max, double min, double Kp, double Kd, double Ki) {
    this->_dt = dt;
    this->_max = max;
    this->_min = min;
    this->_Kp = Kp;
    this->_Kd = Kd;
    this->_Ki = Ki;
}

double pid::calculate(double setpoint, double pv) {
    // Calculate error
    double error = setpoint - pv;

    // Proportional term
    double Pout = _Kp * error;

    // Integral term
    _integral += error * _dt;
    double Iout = _Ki * _integral;

    // Derivative term
    double derivative = (error - _pre_error) / _dt;
    double Dout = _Kd * derivative;

    // Calculate total output
    double output = Pout + Iout + Dout;

    // Restrict to max/min
    if( output > _max )
        output = _max;
    else if( output < _min )
        output = _min;

    // Save error to previous error
    _pre_error = error;

    return output;
}