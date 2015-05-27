#include <ProgmemString.h>
#include <Logging.h>
#include <JsonParser.h>
#include <JsonGenerator.h>
#include <Maxbotix.h>
#include <SimpleTimer.h>

#include "config.h"

using namespace ArduinoJson::Generator;

const float TRAFFIC_COUNTER_VERSION = 2;

//////////////////////////////////////////////////////////////////////////
// Traffic Counter
// 
// Uses non-invasive sensors to count vehicular and pedestrian traffic 
// from a distance. 
//
//////////////////////////////////////////////////////////////////////////
// Config
//////////////////////////////////////////////////////////////////////////

// Rangefinder
Maxbotix rangeSensor(RANGEFINDER_AN_PIN, Maxbotix::AN, Maxbotix::XL);
int rangeBaseline;
int rangeVariance;

int range;

// Timers
SimpleTimer timer;
int rangeTimerID;


//////////////////////////////////////////////////////////////////////////
// Arduino Functions
//////////////////////////////////////////////////////////////////////////

/**
* Startup code - Run once
*/
void setup(){
	// Start up comms
	Log.Init(LOGGER_LEVEL, SERIAL_BAUD);
	Log.Info(P("Traffic Counter - Rangefinder test"));

	startRangefinder();
  
}

/**
* Main loop - run forever
*/
void loop(){
	timer.run();
}


//////////////////////////////////////////////////////////////////////////
// User Functions
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Rangefinder

/**
* startRangefinder
*
* Start the rangefinder sensor
*
* A baseline range (to the ground or static surrounds) is established for
* comparing against new measuremets.
* 
*/
void startRangefinder(){

	// Configure control pint
	enableRangefinder();

	Log.Debug(P("Establishing baseline range..."));

	// Establish baseline (fixed height) of the range sensor
	int averageRange = getRange();
	int baselineReads = 1;
	int averageVariance = 500;

	// Keep reading in the baseline until it stablises
	while (baselineReads < MIN_BASELINE_READS || averageVariance > BASELINE_VARIANCE_THRESHOLD){
		int newRange = getRange();
		int newVariance = (averageRange - newRange);
		if (newVariance < 0){newVariance *= -1;}
		
		averageVariance = ((averageVariance + newVariance)/2);
		averageRange = ((averageRange + newRange)/2);

		Log.Debug(P("Calibration: Range - %d, Variance - %d"), int(averageRange), averageVariance);
		
		baselineReads++;
		delay(BASELINE_READ_INTERVAL);
	}
	
	// Baseline finished
	rangeBaseline = averageRange;
	rangeVariance = averageVariance;

	Log.Debug(P("Baseline established: %d cm, %d cm variance"), averageRange, averageVariance);

	range = averageRange;
	rangeTimerID = timer.setInterval(CHECK_RANGE_INTERVAL, checkRange);

}

/**
* stopRangefinder
*
* Stop reading the ultrasonic range data and disable the sensor
*
*/
void stopRangefinder(){
	disableRangefinder();
	timer.deleteTimer(rangeTimerID);
}

/**
* enabledRangefinder
*
* Allow the ultrasonic rangefinder to send ping signals
*/
void enableRangefinder(){
	pinMode(RANGEFINDER_CONTROL_PIN, OUTPUT);
	digitalWrite(RANGEFINDER_CONTROL_PIN, HIGH);
}

/**
* disableRangefinder
*
* Stop sending range pings.
*/
void disableRangefinder(){
	digitalWrite(RANGEFINDER_CONTROL_PIN, LOW);
}

/**
* getRange
*
* Get the range in cm from the ultrasonic rangefinder
*
* @return Target distance from sensor in cm
*/
int getRange(){
	int targetDistance = rangeSensor.getRange();
	Log.Debug(P("Range: %d cm"), targetDistance);
	return targetDistance;
}

/**
* checkRange
* 
* Check the ultrasonic range sensor to see if a traffic event has occurred.
*
* Traffic events are counted as a break in the sensor's 'view' of the ground.
* Any object between the sensor and the ground baseline will cause the sensor
* to register a shorter range than usual.
*/
void checkRange(){
	int lastRange = range;
	range = getRange();
	
	// Detection occurs when target breaks the LoS to the baseline
	if ((rangeBaseline - range) > RANGE_DETECT_THRESHOLD && (lastRange - range) > RANGE_DETECT_THRESHOLD){
		Log.Debug(P("Detection!"));
	}
}
