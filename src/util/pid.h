//
// Created by Gijs Sijpesteijn on 18/12/2017.
//

#ifndef CARMAGEDDON_PID_H
#define CARMAGEDDON_PID_H


class pid {
public:
    // Kp -  proportional gain
    // Ki -  Integral gain
    // Kd -  derivative gain
    // dt -  loop interval time
    // max - maximum value of manipulated variable
    // min - minimum value of manipulated variable
    pid( double dt, double max, double min, double Kp, double Kd, double Ki );

    // Returns the manipulated variable given a setpoint and current process value
    double calculate( double setpoint, double pv );

    void setKc(double kc);
    void setKi(double ki);
    void setKd(double kd);
    void setMax(double max);
    void setMin(double min);

private:
    double _dt;
    double _max;
    double _min;
    double _Kp;
    double _Kd;
    double _Ki;
    double _pre_error;
    double _integral;
};


#endif //CARMAGEDDON_PID_H
