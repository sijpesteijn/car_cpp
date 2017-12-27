//
// Created by Gijs Sijpesteijn on 06/10/2017.
//

#ifndef CARMAGEDDON_PWM_H
#define CARMAGEDDON_PWM_H

#include <string>

using namespace std;

class PWM {
public:
    PWM(string pwmNr);
    PWM(string pwmNr, int period, int duty_cycle);
    void setPeriod(int period);
    void setDutyCycle(int duty_cycle);
    void setEnable(bool enable);
    bool getEnabled();
private:
    string pwmNr;
    bool enable = false;
    int duty_cycle = 10000000;
    int period     = 20000000;
};

#endif //CARMAGEDDON_PWM_H
