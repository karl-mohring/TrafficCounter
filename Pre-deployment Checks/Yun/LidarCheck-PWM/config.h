#include "Arduino.h"
#include <Logging.h>

// Comms settings
const long SERIAL_BAUD = 115200;
const int LOGGER_LEVEL = LOG_LEVEL_DEBUG;


// Pin assignments
const byte RANGEFINDER_AN_PIN = A0;
const byte RANGEFINDER_CONTROL_PIN = 8;
const byte RANGEFINDER_POWER_PIN = 0;

const byte PIR_MOTION_PIN = 5;
const byte PIR_CONTROL_PIN = 0;

const byte OPTICAL_FLOW_CHIP_SELECT_PIN = 6;

const byte BUZZER_PIN = 12;

const byte LIDAR_TRIGGER_PIN = 0;
const byte LIDAR_DETECT_PIN = 0;
const byte LIDAR_CONTROL_PIN = 0;

const byte YUN_HANDSHAKE_PIN = 7;



// Timer config
const int CHECK_MOTION_INTERVAL = 1000;
const int MOTION_COOLDOWN = 5000; // Time left for the PIR sensor to cool down after a detection in ms

const int CHECK_FLOW_INTERVAL = 200;
const int FLOW_COOLDOWN = 2000; // Optical flow detection cooldown in ms

const int CHECK_RANGE_INTERVAL = 200;
const int RANGE_DETECT_THRESHOLD = 50; // Minimum threshold for range detection in cm. (Target must move at least this much) 

const int LIDAR_CHECK_RANGE_INTERVAL = 200;
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
const int BUZZER_BEEP_QUIET_PERIOD = 50; // Beep 'downtime' in ms
const byte NUM_BEEPS_UVD = 0;
const byte NUM_BEEPS_PIR = 1;
const byte NUM_BEEPS_LIDAR = 2;
const byte NUM_BEEPS_OF = 0;

// JSON string tags
const char ID[] = "id";
const char VERSION[] = "version";
const char COUNT_UVD[] = "count_uvd";
const char UVD_RANGE[] = "uvd_range";
const char COUNT_PIR[] = "count_pir";
const char PIR_STATUS[] = "pir_status";
const char COUNT_OF[] = "count_of";
const char OF_X_OFFSET[] = "of_dx";
const char OF_Y_OFFSET[] = "of_dy";
const char OF_STATUS[] = "of_status";
const char OF_CONNECTED[] = "of_connected";
const char COUNT_LIDAR[] = "count_lidar";
const char LIDAR_RANGE[] = "lidar_range";
