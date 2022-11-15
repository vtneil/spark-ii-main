#ifndef SPARK_II_MAIN_STATES_HPP
#define SPARK_II_MAIN_STATES_HPP

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

#include "spark_tools.hpp"

class State {
protected:
    u8 m_state_id;

public:
    u8 state() const {
        return m_state_id;
    }

    void updateState(u8 newState) {
        m_state_id = newState;
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
private:
    static constexpr u8 DISABLED = 0;
    static constexpr u8 HOLD = 1;
    static constexpr u8 LOG_ID = 2;
    static constexpr u8 READ_EEPROM = 4;
    static constexpr u8 UPDATE_EEPROM = 6;
    static constexpr u8 CLEAR_EEPROM = 8;
public:
    void
};

#endif
