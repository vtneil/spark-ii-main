// SMART TIMER CIRCUIT BOARD SOFTWARE V2
// Vivatsathorn Thitasirivit
// SPARK-II (Software version 0.0.1)
// MAIN CONTROLLER (dev_id = 0)

// Imports
#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_BMP3XX.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>
#include <TinyGPS++.h>

// Define constants
#define SERIAL_BAUD 115200
#define ATGM336H_BAUD 9600
#define LORA_BAUD 115200

// Define pins
#define P_SD_CS 10
#define P_SD_MOSI 11
#define P_SD_MISO 12
#define P_SD_SCK 13

#define P_RX_TEENSY_MAIN 21
#define P_TX_TEENSY_MAIN 20

#define P_RX_ATGM336H 14
#define P_TX_ATGM336H 15

#define P_RX_LORA 0
#define P_TX_LORA 1

// Global objects, constants, variables
const float SEALEVELPRESSURE_HPA = 1013.25;
const float M_G = 9.81;

// Define functionality interval in ms

// RATE_GPS Default: 1000
const u_int16_t RATE_GPS = 1000;

// RATE_SENSOR Default: 500
const u_int16_t RATE_SENSOR = 500;

// RATE_DATALOG Default: 100
const u_int16_t RATE_DATALOG = 100;

// RATE_DATACOM Default: 500
const u_int16_t RATE_DATACOM = 500;

// FIRE_HOLD Default: 2000
const u_int16_t FIRE_HOLD = 2000;

// Define apogee in m, s
const u_int16_t APOGEE_HEIGHT = 900;
const float APOGEE_TIME = 13;
const float APOGEE_THRESHOLD = 0.95;

const String PK_HEADER = "SPARK2,";
const String F_NAME = "S2_MAIN_";
const String F_EXT = ".csv";

// Define device id
const u_int8_t device_id = 0;

Adafruit_BMP3XX bmp388;
SFE_UBLOX_GNSS mainGPS;
TinyGPSPlus secGPS;

// os_state: 0 is default operation, 1 is wait for sd read, 2 is wait for user input
// 3 is sd read, 254 is dfu mode, 255 is end - restart to reset
u_int8_t os_state = 0;
u_int8_t dfu_state = 0;
u_int8_t ss_state = 0;
u_int32_t t_capture;
u_int32_t dt;
u_int32_t counter = 0;
u_int32_t last_millis_sen = 0;
u_int32_t last_millis_gps = 0;
u_int32_t last_millis_log = 0;
u_int32_t last_millis_com = 0;

double gps0_lat, gps0_lon, gps0_alt, gps0_ref_alt, gps0_apogee_alt;
double gps1_lat, gps1_lon, gps1_alt, gps1_ref_alt, gps1_apogee_alt;
float bmp_temp, bmp_pres, bmp_alt, bmp_ref_alt, bmp_apogee_alt;

char file_name[100];
char read_name[100];

String user_inp;
String p_log1 = "";
String p_log2 = "";
String p_com = "";

File root;
File sd_file;
File read_file;

String comma(const String &inp);

float relative(float inp_x, float ref_x);

void printDirectory(File dir, uint8_t num_tabs);

void deleteAllFiles(File dir);

void setup() {
    // PIN
    pinMode(LED_BUILTIN, OUTPUT);

    // SERIAL
    Serial.begin(SERIAL_BAUD);
    Serial5.begin(SERIAL_BAUD);
    Serial1.begin(LORA_BAUD);
    Serial3.begin(ATGM336H_BAUD);

    // I2C
    Wire.begin();
    Wire.setClock(400000);

    // SPI
    SPI.setMISO(P_SD_MISO);
    SPI.setMOSI(P_SD_MOSI);
    SPI.setSCK(P_SD_SCK);
    SPI.setCS(P_SD_CS);

    // SD
    if (!SD.begin(BUILTIN_SDCARD)) {
        SD.begin(P_SD_CS);
    }
    int sd_file_idx = 0;
    while (true) {
        String file_name_str = (F_NAME + String(sd_file_idx) + F_EXT);
        file_name_str.toCharArray(file_name, 100);
        sd_file_idx++;
        if (!SD.exists(file_name)) break;
    }
    sd_file = SD.open(file_name, FILE_WRITE);

    // BMP388
    bmp388.begin_I2C();
    bmp388.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
    bmp388.setPressureOversampling(BMP3_OVERSAMPLING_4X);
    bmp388.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
    bmp388.setOutputDataRate(BMP3_ODR_50_HZ);
    bmp_ref_alt = bmp388.readAltitude(SEALEVELPRESSURE_HPA);

    // MAIN GPS
    mainGPS.begin();
    mainGPS.setI2COutput(COM_TYPE_UBX);
    mainGPS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT);
    gps0_ref_alt = mainGPS.getAltitude();

    delay(1000);
}

void loop() {
    if (Serial.available() > 0) {
        user_inp = Serial.readString();
        if (user_inp == "CHECK_STATE") {
            Serial.println(os_state);
            Serial.println(ss_state);
            Serial.println(dfu_state);
        } else if (os_state == 2) {
            if (user_inp == "END_READ") {
                os_state = 255;
            } else {
                os_state = 3;
            }
        } else if (os_state == 254) {
            if (dfu_state != 0) {
                if (user_inp == "SYS_CMD_REQUEST_DEVICE_ID") {
                    dfu_state = 2;
                } else if (user_inp == "READ_EEPROM") {
                    dfu_state = 4;
                } else if (user_inp == "WRITE_EEPROM") {
                    dfu_state = 6;
                } else if (user_inp == "SYS_ADMIN_CMD_CLEAR_EEPROM") {
                    dfu_state = 8;
                } else if (user_inp == "EXIT_DFU") {
                    os_state = 0;
                    dfu_state = 0;
                }
            }

        } else if (user_inp == "START_OP") {
            os_state = 0;
        } else if (user_inp == "READ_SD") {
            os_state = 1;
        } else if (user_inp == "SYS_ADMIN_CMD_DELETION_ALL_SD") {
            os_state = 4;
        } else if (user_inp == "ENTER_DFU") {
            os_state = 254;
            dfu_state = 1;
        } else if (user_inp == "END_OP") {
            os_state = 255;
        }

    }

    // operational mode
    if (os_state == 0) {
        digitalWrite(LED_BUILTIN, HIGH);
        t_capture = millis();
        if (t_capture - last_millis_sen >= RATE_SENSOR) {
            if (bmp388.performReading()) {
                bmp_temp = (float) bmp388.temperature;
                bmp_pres = (float) bmp388.pressure;
                bmp_pres /= 100.0;
                bmp_alt = bmp388.readAltitude(SEALEVELPRESSURE_HPA);
            }
            last_millis_sen = millis();
        }

        t_capture = millis();
        if (t_capture - last_millis_gps >= RATE_GPS) {
            gps0_lat = mainGPS.getLatitude() / pow(10, 7);
            gps0_lon = mainGPS.getLongitude() / pow(10, 7);
            gps0_alt = mainGPS.getAltitude();

            if (Serial3.available() > 0) {
                int sec_gps_data = Serial3.read();
                if (secGPS.encode(sec_gps_data)) {
                    gps1_lat = secGPS.location.lat();
                    gps1_lon = secGPS.location.lng();
                    gps1_alt = secGPS.altitude.meters();
                }
            }

            last_millis_gps = millis();
        }

        t_capture = millis();
        if (t_capture - last_millis_com >= RATE_DATACOM) {
            p_com = PK_HEADER;
            p_com += comma(String(counter));
            p_com += comma(String(gps0_lat));
            p_com += comma(String(gps0_lon));
            p_com += comma(String(gps0_alt));
            p_com += comma(String(gps1_lat));
            p_com += comma(String(gps1_lon));
            p_com += comma(String(gps1_alt));
            p_com += comma(String(bmp_temp));
            p_com += comma(String(bmp_pres));
            p_com += comma(String(bmp_alt));

            Serial1.println(p_com);

            last_millis_com = millis();
        }

        t_capture = millis();
        if (t_capture - last_millis_log >= RATE_DATALOG) {
            p_log1 = PK_HEADER;
            p_log1 += comma(String(counter));
            p_log1 += comma(String(gps0_lat));
            p_log1 += comma(String(gps0_lon));
            p_log1 += comma(String(gps0_alt));
            p_log1 += comma(String(gps1_lat));
            p_log1 += comma(String(gps1_lon));
            p_log1 += comma(String(gps1_alt));
            p_log1 += comma(String(bmp_temp));
            p_log1 += comma(String(bmp_pres));
            p_log1 += comma(String(bmp_alt));

            Serial.println(p_log1);

            if (!sd_file) {
                sd_file = SD.open(file_name, FILE_WRITE);
            }
            if (sd_file) {
                sd_file.print(p_log1);
                sd_file.flush();
            }

            counter++;
            last_millis_log = millis();
        }
        digitalWrite(LED_BUILTIN, LOW);
    }

        // sd read wait mode
    else if (os_state == 1) {
        root = SD.open("/");
        Serial.println(F("-------------"));
        printDirectory(root, 0);
        Serial.println(F("-------------"));
        os_state = 2;
    }

        // sd read inp
    else if (os_state == 3) {
        user_inp.toCharArray(read_name, 100);
        read_file = SD.open(read_name, FILE_READ);
        if (read_file) {
            Serial.println("OBJ_SD_START_READ " + String(read_file.name()));
            while (read_file.available()) {
                Serial.write(read_file.read());
            }
            Serial.println();
            Serial.println("OBJ_SD_END_READ");
            read_file.close();
        }
        os_state = 2;

    } else if (os_state == 4) {
        while (Serial.available() <= 0);
        user_inp = Serial.readString();
        if (user_inp == "CONFIRM_SYS_ADMIN_CMD_DELETION_ALL_SD") {
            root = SD.open("/");
            Serial.println(F("-------------"));
            deleteAllFiles(root);
            Serial.println(F("-------------"));
        }
        os_state = 0;
    }

        // dfu
    else if (os_state == 254) {
        if (dfu_state == 2) {
            Serial.print("SYS_RESPOND_DEVICE_ID_");
            Serial.println(device_id);

        } else if (dfu_state == 4) {
            while (Serial.available() <= 0);
            user_inp = Serial.readString();
            if (user_inp.length() > 0) {
                int arg_read_addr = user_inp.toInt();
                int eeprom_read = EEPROM.read(arg_read_addr);
                Serial.println(eeprom_read);
            }

        } else if (dfu_state == 6) {
            while (Serial.available() <= 0);
            user_inp = Serial.readString();
            if (user_inp.length() > 0) {
                int arg_write_addr = user_inp.toInt();
                while (Serial.available() <= 0);
                user_inp = Serial.readString();
                if (user_inp.length() > 0) {
                    int arg_write_val = user_inp.toInt();
                    EEPROM.update(arg_write_addr, arg_write_val);
                }
            }

        } else if (dfu_state == 8) {
            while (Serial.available() <= 0);
            user_inp = Serial.readString();
            if (user_inp == "CONFIRM_SYS_ADMIN_CMD_CLEAR_EEPROM") {
                // TEENSY 4.0 CLEAR 0 to 1079
                for (int idx_eepr_clear = 0; idx_eepr_clear < 1080; idx_eepr_clear++) {
                    EEPROM.update(idx_eepr_clear, 0);
                }
            }
        }
        dfu_state = 1;
    }
}

String comma(const String &inp) {
    return (inp + ",");
}

float relative(float inp_x, float ref_x) {
    return (inp_x - ref_x);
}

void printDirectory(File dir, uint8_t num_tabs) {
    File entry;
    while (true) {
        entry = dir.openNextFile();
        if (!entry) break;
        for (uint8_t i = 0; i < num_tabs; i++) {
            Serial.print("\t");
        }
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

void deleteAllFiles(File dir) {
    File entry;
    while (true) {
        entry = dir.openNextFile();
        if (!entry) break;
        if (!entry.isDirectory()) {
            SD.remove(entry.name());
        }
        entry.close();
    }
}
