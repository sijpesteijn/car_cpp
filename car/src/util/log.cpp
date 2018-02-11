//
// Created by Gijs Sijpesteijn on 20/12/2017.
//

#include "log.h"
#include <iostream>
#include <syslog.h>

using namespace std;

void log::debug(string message) {
#ifdef __APPLE__
    cout << message << endl;
#else
    syslog (LOG_DEBUG, "%s", message.c_str());
#endif
}