#include "Arduino.h"

// Comms settings
const long SERIAL_BAUD = 57600;
const int LOGGER_LEVEL = LOG_LEVEL_DEBUG;

// Pin assignments
const byte RANGEFINDER_AN_PIN = A0;
const byte RANGEFINDER_CONTROL_PIN = A1;
const byte PIR_MOTION_PIN = A7;
const byte PIR_CONTROL_PIN = A9;

const byte BUZZER_PIN = 2;

// Timer config
const int CHECK_MOTION_INTERVAL = 500;
const int MOTION_TIMEOUT = 3000;

const int CHECK_RANGE_INTERVAL = 200;
const int RANGE_DETECT_THRESHOLD = 50; // Minimum threshold for range detection in cm. (Target must move at least this much) 

const int BUZZER_BEEP_DURATION = 100;

// Misc

const byte COMMAND_CACHE_SIZE = 80;
const byte MIN_BASELINE_READS = 15; // Minimum number of reads needed to establish a baseline
const int BASELINE_READ_INTERVAL = 200; // Time between range baseline calibration reads
const int BASELINE_VARIANCE_THRESHOLD = 5; // Maximum acceptable range sensor baseline error in cm.

const int MOTION_INITIALISATION_TIME = 5000; // Time given for PIR sensor to calibrate in ms.
const byte MOTION_DETECTED = HIGH;

// Commands
const char COMMAND_TERMINATOR = '$';
const char RESET_UVD_COUNT = 'q';
const char DISABLE_UVD = 'w';
const char ENABLE_UVD = 'e';

