//
// Created by Gijs Sijpesteijn on 05/12/2017.
//

#ifndef CARMAGEDDON_SETTINGS_H
#define CARMAGEDDON_SETTINGS_H

#include <string>
#include <jansson.h>
#include <fstream>

class settings {
public:
    settings(std::string settings_file);
    ~settings();
    void save(json_t *);
    json_t *getSettings();

private:
    json_t* root;
    std::fstream file_stream;
    std::string settings_file;
};


#endif //CARMAGEDDON_SETTINGS_H
