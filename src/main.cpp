// SMART TIMER CIRCUIT BOARD SOFTWARE V2
// Vivatsathorn Thitasirivit
// SPARK-II (Software version 0.0.1)
// MAIN CONTROLLER (dev_id = 0)

// Imports
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP3XX.h>

// Define constants
#define SERIAL_BAUD 115200

// Define pins
#define P_X A7
#define P_Y A8
#define P_Z A9

#define P_SD_CS 10
#define P_SD_MOSI 11
#define P_SD_MISO 12
#define P_SD_SCK 13

#define P_WD_SCL 19
#define P_WD_SDA 18
#define P_WD_TX 17
#define P_WD_RX 16

#define P_RX_TEENSY_MAIN 1
#define P_TX_TEENSY_MAIN 0

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

Adafruit_BMP3XX bmp388;

// os_state: 0 is default operation, 1 is wait for sd read, 2 is wait for user input
// 3 is sd read, 255 is end - restart to reset
u_int8_t os_state = 0;
u_int8_t ss_state = 0;
u_int32_t t_capture;
u_int32_t dt;
u_int32_t counter = 0;
u_int32_t last_millis_imu = 0;
u_int32_t last_millis_log = 0;
u_int32_t last_millis_com = 0;

float gps_lat, gps_lon, gps_alt;
float bmp_temp, bmp_pres, bmp_alt, bmp_ref_alt;

char file_name[100];
char read_name[100];

String p_log1 = "";
String p_log2 = "";
String p_com = "";

File root;
File sd_file;
File read_file;

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}