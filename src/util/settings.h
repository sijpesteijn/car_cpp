//
// Created by Gijs Sijpesteijn on 05/12/2017.
//

#ifndef CARMAGEDDON_SETTINGS_H
#define CARMAGEDDON_SETTINGS_H


#include <jansson.h>

class settings {
public:
    settings(const char *settings_file);
    ~settings();
    void save(json_t *);
    json_t *getSettings();

private:
    json_t* root;
    const char *settings_file;
};


#endif //CARMAGEDDON_SETTINGS_H
