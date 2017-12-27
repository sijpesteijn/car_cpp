//
// Created by Gijs Sijpesteijn on 05/12/2017.
//

#include <cstdio>
#include "settings.h"

using namespace std;

settings::settings(string settings_file) {
    this->settings_file = settings_file;
//    this->file_stream.open(settings_file.c_str())
    FILE *settingsFile = fopen(settings_file.c_str(), "r+");
    json_error_t error;

    root = json_loadf(settingsFile, 0, &error);
    if (!root) {
        fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
    }
    if (!json_is_object(root)) {
        fprintf(stderr, "error: commit data is not an object\n");
        json_decref(root);
    }
    fclose(settingsFile);
}

settings::~settings() {

}

void settings::save(json_t *settings) {
    FILE *settingsFile = fopen(settings_file.c_str(), "w+");
    root = settings;
    json_dumpf(root, settingsFile, JSON_INDENT(5));
    fclose(settingsFile);
}

json_t *settings::getSettings() {
    return root;
}