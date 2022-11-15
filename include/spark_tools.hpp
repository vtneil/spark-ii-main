#ifndef SPARK_II_MAIN_SPARK_TOOLS_HPP
#define SPARK_II_MAIN_SPARK_TOOLS_HPP

#include <Arduino.h>
#include "definitions.hpp"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

class charBuffer {
protected:
    char *buffer;

public:
    explicit charBuffer(uint8_t length) : buffer(new char[length]) {}

    ~charBuffer() {
        delete[] buffer;
    }
};

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
