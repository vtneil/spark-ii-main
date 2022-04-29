# spark-ii-main
An Arduino-framework-based Teensy 4.0 software for SPARK-II rocket electronics for receivin and data logging telemetry of sensor data.

## Features
This software utilizes pseudo-asynchronous process to operate each type of operation in different phases and intervals. It also features Operational Mode, Simulation Mode (*Coming soon*), On-board SD Card Reading and DFU Mode (EEPROM Read/Write) via Serial command line.

## Commands and System States
There are three integers specifying the state of software:
#### 1. OS_STATE
`os_state` is for controlling software operation mode. Available states are:
1. `0` Operational State
2. `1` SD Directory listing
3. `2` Wait for User Input -> Filename
4. `3` Serial log Filename's File content
5. `253` Simulation mode (*Coming soon*)
6. `254` DFU Mode
7. `255` End of operation
