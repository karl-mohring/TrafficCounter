#include "Arduino.h"
#include <Logging.h>

// Comms settings
const long SERIAL_BAUD = 57600;
const int LOGGER_LEVEL = LOG_LEVEL_DEBUG;


// Pin assignments
const byte RANGEFINDER_AN_PIN = A0;
const byte RANGEFINDER_CONTROL_PIN = A1;

const byte PIR_MOTION_PIN = A9;
const byte PIR_CONTROL_PIN = A7;

const byte OPTICAL_FLOW_CHIP_SELECT_PIN = 41;

const byte BUZZER_PIN = 38;

const byte LIDAR_TRIGGER_PIN = A13;
const byte LIDAR_DETECT_PIN = A14;
const byte LIDAR_CONTROL_PIN = 0;



// Timer config
const int CHECK_MOTION_INTERVAL = 1000;
const int MOTION_COOLDOWN = 5000; // Time left for the PIR sensor to cool down after a detection in ms

const int CHECK_FLOW_INTERVAL = 200;
const int FLOW_COOLDOWN = 2000; // Optical flow detection cooldown in ms

const int CHECK_RANGE_INTERVAL = 100;
const int RANGE_DETECT_THRESHOLD = 50; // Minimum threshold for range detection in cm. (Target must move at least this much) 

const int LIDAR_CHECK_RANGE_INTERVAL = 100;
const int LIDAR_DETECT_THRESHOLD = 50;

const long LOG_TIMER_INTERVAL = 5000; // Time in between SD card log entries in ms.

const long XBEE_TRANSMIT_INTERVAL = 30000; // Time between XBee transmissions

// Misc

const byte COMMAND_CACHE_SIZE = 80;
const byte SEND_BUFFER_SIZE = 50;
const byte MIN_BASELINE_READS = 15; // Minimum number of reads needed to establish a baseline
const int BASELINE_READ_INTERVAL = 200; // Time between range baseline calibration reads
const int BASELINE_VARIANCE_THRESHOLD = 5; // Maximum acceptable range sensor baseline error in cm.

const int MOTION_INITIALISATION_TIME = 3000; // Time given for PIR sensor to calibrate in ms.
const byte MOTION_DETECTED = HIGH;

const byte OPTICAL_FLOW_MAX_RETRIES = 8;


// Commands
const char COMMAND_TERMINATOR = '$';
const char RESET_UVD_COUNT = 'q';
const char DISABLE_UVD = 'w';
const char ENABLE_UVD = 'e';

// Buzzer
const int BUZZER_BEEP_PERIOD = 10; // Beep length in ms
const byte NUM_BEEPS_UVD = 2;
const byte NUM_BEEPS_PIR = 1;
const byte NUM_BEEPS_LIDAR = 0;
const byte NUM_BEEPS_OF = 4;

