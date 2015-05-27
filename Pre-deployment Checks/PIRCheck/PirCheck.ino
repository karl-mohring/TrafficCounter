#include <ProgmemString.h>
#include <SimpleTimer.h>
#include <Logging.h>
#include "config.h"

SimpleTimer timer;
int motionTimerID = -1;

bool motionDetected;
int motionCount;
long detectionStart;

void setup(){
	Log.Init(LOGGER_LEVEL, SERIAL_BAUD);
	Log.Info(P("Traffic Counter - PIR Test"));

	detectionStart = 0;
	startMotionDetector();

}

void loop()
{

	timer.run();

}


//////////////////////////////////////////////////////////////////////////
// Motion Detector

/**
* startMotionDetector
*
* Start the PIR motion sensor for motion detections
*/
void startMotionDetector(){
	enableMotion();

	pinMode(PIR_MOTION_PIN, INPUT);

	Log.Debug(P("Calibrating motion sensor - wait %d ms"), MOTION_INITIALISATION_TIME);
	delay(MOTION_INITIALISATION_TIME);
	motionDetected = false;

	motionTimerID = timer.setInterval(CHECK_MOTION_INTERVAL, getMotion);
	Log.Debug(P("Motion started"));
}

/**
* enableMotion
*
* Turn on the motion sensor
*/
void enableMotion(){
	pinMode(PIR_CONTROL_PIN, OUTPUT);
	digitalWrite(PIR_CONTROL_PIN, HIGH);
}

/**
* stopMotionDetector
*
* Stop checking the motion detector
*/
void stopMotionDetector(){
	timer.deleteTimer(motionTimerID);
	disableMotion();

	Log.Debug(P("Motion sensor stopped"));
}

/**
* Check the PIR sensor for detected movement
* The sensor will output HIGH when motion is detected.
* Detections will hold the detection status HIGH until the cool-down has lapsed (default: 60s)
*
* Returns:
*	True if motion has been detected recently
*/
void getMotion(){

	// Check the sensor
	if (digitalRead(PIR_MOTION_PIN) == MOTION_DETECTED){
		if (motionDetected == false){
			detectionStart = millis();
		}
		motionDetected = true;
		

	}
	else{
		// No alarm; is fine
		motionDetected = false;
		detectionStart = millis();

	}

	long timeSinceDetection = millis() - detectionStart;
	Log.Info(P("Motion detected: %T - %l ms"), motionDetected, timeSinceDetection);
}

/**
* Temporarily pause checking the PIR motion sensor
*/
void motionCoolDown(){
	timer.deleteTimer(motionTimerID);
	motionTimerID = -1;
	timer.setTimeout(MOTION_TIMEOUT, resumeMotionDetection);
}

/**
* Start polling the PIR motion sensor
*/
void resumeMotionDetection(){
	motionTimerID = timer.setInterval(CHECK_MOTION_INTERVAL, getMotion);
}

/**
* resetMotionCount
*
* Reset the number of motion detections
*/
void resetMotionCount(){
	motionCount = 0;
}

/**
* disableMotion
*
* Turn off the motion sensor
*/
void disableMotion(){
	digitalWrite(PIR_CONTROL_PIN, LOW);
}