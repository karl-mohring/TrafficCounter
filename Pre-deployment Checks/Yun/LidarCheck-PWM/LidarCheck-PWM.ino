#include "Arduino.h"
#include <I2C.h>
#include <ProgmemString.h>
#include <SimpleTimer.h>
#include <Logging.h>
#include <LIDARLite_registers.h>
#include <LIDARduino.h>
#include "config.h"


bool linuxBusy;

LIDAR_Lite_PWM lidar(4, 5);
SimpleTimer timer;
int lidarTimerID;

int lidarBaselineRange = 0;
int lidarRange = 0;
int lidarCount = 0;

void setup()
{
	startYunSerial();
	startLidar();
}

void loop()
{
	timer.run();
}


//////////////////////////////////////////////////////////////////////////
// Yun Serial

/**
* Set up the Arduino-Linux serial bridge
* Serial output from the Arduino is disabled until the Yun has finished booting
*/
void startYunSerial(){

	// Set up the handshake pin
	pinMode(YUN_HANDSHAKE_PIN, INPUT_PULLUP);
	pinMode(LED_BUILTIN, OUTPUT);

	Serial1.begin(115200);

	// Check the initial state of the handshake pin (LOW == Ready)
	_bootStatusChange();

	// Listen on the handshake pin for any changes
	attachInterrupt(4, _bootStatusChange, CHANGE);
}

/**
* Check the boot status of the Yun
*/
void _bootStatusChange(){
	linuxBusy = digitalRead(YUN_HANDSHAKE_PIN);
	digitalWrite(LED_BUILTIN, linuxBusy);

	// Disable log output until Linux boots
	if (linuxBusy){
		Log.Init(LOG_LEVEL_NOOUTPUT, &Serial1);
	}
	else{
		Log.Init(LOGGER_LEVEL, &Serial1);
	}
}


//////////////////////////////////////////////////////////////////////////
// Lidar

/**
* Start the rangefinder sensor
*
* A baseline range (to the ground or static surrounds) is established for
* comparing against new measuremets.
*
*/
void startLidar(){
	pinMode(LIDAR_TRIGGER_PIN, INPUT);
	pinMode(LIDAR_DETECT_PIN, INPUT);
	pinMode(LIDAR_CONTROL_PIN, OUTPUT);

	lidar.begin();
	enableLidar();

	Log.Debug(P("Lidar - Establishing baseline range..."));
	lidarBaselineRange = getLidarBaselineRange(BASELINE_VARIANCE_THRESHOLD);
	Log.Debug(P("Lidar Baseline established: %d cm, %d cm variance"), lidarBaselineRange, BASELINE_VARIANCE_THRESHOLD);

	lidarTimerID = timer.setInterval(LIDAR_CHECK_RANGE_INTERVAL, checkLidarRange);
}

/**
* Establish the baseline range from the lidar to the ground
* The sensor will take samples until the readings are consistent
*/
int getLidarBaselineRange(int variance){
	// Establish baseline (fixed height) of the range sensor
	int averageRange = getLidarRange();
	int baselineReads = 1;
	int averageVariance = 500;

	// Keep reading in the baseline until it stablises
	while (baselineReads < MIN_BASELINE_READS || averageVariance > variance){
		int newRange = getLidarRange();
		int newVariance = (averageRange - newRange);
		if (newVariance < 0){ newVariance *= -1; }

		averageVariance = ((averageVariance + newVariance) / 2);
		averageRange = ((averageRange + newRange) / 2);

		Log.Debug(P("Lidar Calibration: Range - %d, Variance - %d"), int(averageRange), averageVariance);

		baselineReads++;
		delay(BASELINE_READ_INTERVAL);
	}

	return averageRange;
}

/**
* Stop reading the lidar and disable the sensor
*/
void stopLidar(){
	disableLidar();
	timer.deleteTimer(lidarTimerID);
}

/**
* Allow the lidar to send ping signals
*/
void enableLidar(){
	digitalWrite(LIDAR_CONTROL_PIN, HIGH);
}

/**
* Stop sending range pings.
*/
void disableLidar(){
	digitalWrite(LIDAR_CONTROL_PIN, LOW);
}

/**
* Get the range in cm from the lidar
*
* @return Target distance from sensor in cm
*/
int getLidarRange(){
	int targetDistance = lidar.getDistance();
	Log.Debug(P("Lidar Range: %d cm"), targetDistance);

	return targetDistance;
}

/**
* Check the lidar to see if a traffic event has occurred.
*
* Traffic events are counted as a break in the sensor's 'view' of the ground.
* Any object between the sensor and the ground baseline will cause the sensor
* to register a shorter range than usual.
*/
void checkLidarRange(){
	int lastRange = lidarRange;
	int newRange = getLidarRange();

	lidarRange = newRange;

	// Detection occurs when target breaks the LoS to the baseline
	if ((lidarBaselineRange - newRange) > RANGE_DETECT_THRESHOLD && (lastRange - newRange) > RANGE_DETECT_THRESHOLD){

		// Increase traffic count
		lidarCount++;

		Log.Debug(P("Traffic count - Lidar: %d counts"), lidarCount);
	}
}

/**
* Reset the traffic count for the lidar
*/
void resetLidarCount(){
	lidarCount = 0;
}
