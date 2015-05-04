#include <ProgmemString.h>
#include <StraightBuffer.h>
#include <PirMotion.h>
#include <Logging.h>
#include <JsonParser.h>
#include <JsonGenerator.h>
#include <Maxbotix.h>
#include <SimpleTimer.h>
#include <CommandHandler.h>
#include "config.h"

using namespace ArduinoJson::Generator;

const float TRAFFIC_COUNTER_VERSION = 0.1;

//////////////////////////////////////////////////////////////////////////
// Traffic Counter
// 
// Uses non-invasive sensors to count vehicular and pedestrian traffic 
// from a distance. 
//
//////////////////////////////////////////////////////////////////////////
// Config
//////////////////////////////////////////////////////////////////////////

Maxbotix rangeSensor(RANGEFINDER_AN_PIN, Maxbotix::AN, Maxbotix::XL);
float rangeBaseline;
float rangeVariance;

SimpleTimer timer;
int rangeTimerID;
int motionTimerID;

JsonObject<3> trafficData;
char _commandCache[COMMAND_CACHE_SIZE];
CommandHandler commandHandler(_commandCache, COMMAND_CACHE_SIZE);
//////////////////////////////////////////////////////////////////////////
// Arduino Functions
//////////////////////////////////////////////////////////////////////////

void setup()
{

  // Start up comms
  Log.Init(LOGGER_LEVEL, SERIAL_BAUD);
  Log.Info(P("Traffic Counter - ver %d"), TRAFFIC_COUNTER_VERSION);
  
  // Initialise SD card
  
  // Start up sensors
  startSensors();
  startBuzzer();

}

void loop()
{
	timer.run();
	checkSerial();

  /* add main program code here */

}

//////////////////////////////////////////////////////////////////////////
// User Functions
//////////////////////////////////////////////////////////////////////////

void startSensors(){
	startRangefinder();
	startMotionDetector();
}

void checkSerial(){
	while (Serial.available()){
		char inChar = Serial.read();
		commandHandler.readIn(inChar);
	}
}

//////////////////////////////////////////////////////////////////////////
// Rangefinder

void startRangefinder(){
	
	// Establish baseline (fixed height) of the range sensor
	float averageRange = getRange();
	int baselineReads = 1;
	float averageVariance = 0;
	while (baselineReads < MIN_BASELINE_READS && averageVariance < BASELINE_VARIANCE_THRESHOLD){
		float newRange = getRange();
		float newVariance = abs(averageRange - newRange);
		
		averageVariance = ((averageVariance + newVariance)/2.0);
		averageRange = ((averageRange + newRange)/2.0);
		
		delay(BASELINE_READ_INTERVAL);
	}
	
	// Baseline finished
	rangeBaseline = averageRange;
	rangeVariance = averageVariance;	
		
}

float getRange(){
	
}

//////////////////////////////////////////////////////////////////////////
// Motion Detector

void startMotionDetector(){
	
}

bool getMotion(){
	
}

//////////////////////////////////////////////////////////////////////////
// Buzzer

void startBuzzer(){
	
}

void buzzerOn(){
	
}

void buzzerOff(){
	
}

