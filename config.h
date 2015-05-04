#include "Arduino.h"

// Comms settings
const long SERIAL_BAUD = 57600;
const int LOGGER_LEVEL = LOG_LEVEL_INFOS;

// Pin assignments
const byte RANGEFINDER_AN_PIN = A0;
const byte RANGEFINDER_RX_PIN = 1;
const byte PIR_MOTION_PIN = A1;

const byte BUZZER_PIN = 2;

// Timer config
const int CHECK_MOTION_INTERVAL = 500;
const int MOTION_TIMEOUT = 1000;

const int CHECK_RANGE_INTERVAL = 200;
const float RANGE_DETECT_THRESHOLD = 0.5; 

const int BUZZER_BEEP_DURATION = 100;

// Misc

const byte COMMAND_CACHE_SIZE = 80;
const byte MIN_BASELINE_READS = 5; // Minimum number of reads needed to establish a baseline
const int BASELINE_READ_INTERVAL = 200; // Time between range baseline calibration reads
const float BASELINE_VARIANCE_THRESHOLD = 0.1; // Maximum acceptable range sensor baseline error in m.


