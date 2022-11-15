#ifndef SPARK_II_MAIN_STATES_H
#define SPARK_II_MAIN_STATES_H

#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_BMP3XX.h>
#include <SparkFun_Ublox_Arduino_Library.h>
#include <TinyGPS++.h>

#include "spark_tools.h"

class State {
protected:
    u_int8_t m_state;

public:
    u_int8_t state() const {
        return m_state;
    }

    void updateState(u_int8_t newState) {
        m_state = newState;
    }
};

class HardwareState : public State {
protected:
    GPSData gpsData;

public:

};


class SoftwareState : public State {

};

class DFUState : public State {

};

#endif
