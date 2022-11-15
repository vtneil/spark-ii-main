#ifndef SPARK_II_MAIN_SPARK_TOOLS_H
#define SPARK_II_MAIN_SPARK_TOOLS_H

#include <Arduino.h>
#include "definitions.h"

struct GPSData {
    double latitude;
    double longitude;
    double altitude;
};

struct EnvData {
    double temperature;
    double pressure;
    double altitude;
};

String comma(const String &inp) {
    return inp + ",";
}

float relative(float inp_x, float ref_x = 0) {
    return inp_x - ref_x;
}

void printDirectory(File &dir, const uint8_t &num_tabs) {
    File entry;
    while (true) {
        entry = dir.openNextFile();
        if (!entry) return;

        for (uint8_t i = 0; i < num_tabs; ++i) Serial.print("\t");
        Serial.print(entry.name());

        if (entry.isDirectory()) {
            Serial.println("/");
            printDirectory(entry, num_tabs + 1);
        } else {
            Serial.print("\t\t");
            Serial.println(entry.size(), DEC);
        }
        entry.close();
    }
}

void deleteAllFiles(File &dir) {
    File entry;
    while (true) {
        entry = dir.openNextFile();
        if (!entry) return;
        if (!entry.isDirectory()) SD.remove(entry.name());
        entry.close();
    }
}

#endif
