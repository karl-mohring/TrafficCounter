#include <I2C.h>
#include <LIDARLite_registers.h>
#include <LIDARduino.h>
#include <ADNS3080.h>
#include <SPI\SPI.h>
#include <ProgmemString.h>
#include <StraightBuffer.h>
#include <Logging.h>
#include <JsonParser.h>
#include <JsonGenerator.h>
#include <Maxbotix.h>
#include <SimpleTimer.h>
#include <CommandHandler.h>

#include "config.h"

using namespace ArduinoJson::Generator;

const int TRAFFIC_COUNTER_VERSION = 4;

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

// Lidar
LIDAR_Lite_I2C lidar;
int lidarBaselineRange;

// PIR motion
bool motionDetected;
long timeOfLastMotion;

// Optical Flow
AP_OpticalFlow_ADNS3080 flowSensor(OPTICAL_FLOW_CHIP_SELECT_PIN, 0);
int8_t flow_dx;
int8_t flow_dy;

// Timers
SimpleTimer timer;
int rangeTimerID = -1;
int motionTimerID = -1;
int flowTimerID = -1;
int lidarTimerID = -1;

// Buzzer stuff
int beepCount = 0;
int maxBeeps = 0;

// Data storage
JsonObject<13> trafficEntry;
StraightBuffer sendBuffer(SEND_BUFFER_SIZE);

// Serial commands
char _commandCache[COMMAND_CACHE_SIZE];
CommandHandler commandHandler(_commandCache, COMMAND_CACHE_SIZE);

long entryNumber;


//////////////////////////////////////////////////////////////////////////
// Arduino Functions
//////////////////////////////////////////////////////////////////////////

/**
* Startup code - Run once
*/
void setup()
{
	// Start up comms
	Log.Init(LOGGER_LEVEL, SERIAL_BAUD);
	Log.Info(P("Traffic Counter - ver %d"), TRAFFIC_COUNTER_VERSION);

	// Start up sensors
	startBuzzer();
	startSensors();
	addSerialCommands();

}

/**
* Main loop - run forever
*/
void loop()
{
	timer.run();
	checkSerial();
}


//////////////////////////////////////////////////////////////////////////
// User Functions
//////////////////////////////////////////////////////////////////////////

/**
* Enable and configure the sensor suite for reading
*/
void startSensors(){
	trafficEntry[ID] = "trafficCount";
	trafficEntry[VERSION] = TRAFFIC_COUNTER_VERSION;

	trafficEntry[COUNT_UVD] = 0;
	trafficEntry[UVD_RANGE] = 0;

	trafficEntry[COUNT_PIR] = 0;
	trafficEntry[PIR_STATUS] = false;

	trafficEntry[COUNT_OF] = 0;
	trafficEntry[OF_X_OFFSET] = 0;
	trafficEntry[OF_Y_OFFSET] = 0;
	trafficEntry[OF_STATUS] = false;
	trafficEntry[OF_CONNECTED] = false;

	trafficEntry[COUNT_LIDAR] = 0;
	trafficEntry[LIDAR_RANGE] = 0;

	startRangefinder();
	startLidar();
	startMotionDetector();
	startOpticalFlow();
}

/**
* Check serial comms for incoming commands
*/
void checkSerial(){
	while (Serial.available()){
		char inChar = Serial.read();
		commandHandler.readIn(inChar);
	}
}

/**
* Add commands to the response list
*/
void addSerialCommands(){
	commandHandler.setTerminator(COMMAND_TERMINATOR);

	commandHandler.addCommand(RESET_UVD_COUNT, resetRangeCount);
	commandHandler.addCommand(DISABLE_UVD, startRangefinder);
	commandHandler.addCommand(ENABLE_UVD, stopRangefinder);

	commandHandler.setDefaultHandler(defaultHandler);
}

/**
* Bad command received. Make fun of the sender
*/
void defaultHandler(char c){
	Log.Error(P("Command not recognised: %s"), c);
}

/**
* Print the current traffic counts and info to Serial
*/
void printData(){
	Serial.print("#");
	trafficEntry.printTo(Serial);
	Serial.println("$");
}


//////////////////////////////////////////////////////////////////////////
// Rangefinder

/**
* Start the rangefinder sensor
*
* A baseline range (to the ground or static surrounds) is established for
* comparing against new measuremets.
*
*/
void startRangefinder(){

	// Configure control pint
	enableRangefinder();

	Log.Debug(P("Ultrasonic - Establishing baseline range..."));
	rangeBaseline = getRangefinderBaseline(BASELINE_VARIANCE_THRESHOLD);

	Log.Debug(P("Ultrasonic Baseline established: %d cm, %d cm variance"), rangeBaseline, BASELINE_VARIANCE_THRESHOLD);
	trafficEntry[UVD_RANGE] = rangeBaseline;
	printData();

	rangeTimerID = timer.setInterval(CHECK_RANGE_INTERVAL, checkRange);
}

/**
* Establish the baseline range from the sensor to the ground
* The sensor will take samples until the readings are consistent
*/
int getRangefinderBaseline(int variance){
	// Establish baseline (fixed height) of the range sensor
	int averageRange = getRange();
	int baselineReads = 1;
	int averageVariance = 500;

	// Keep reading in the baseline until it stablises
	while (baselineReads < MIN_BASELINE_READS || averageVariance > variance){
		int newRange = getRange();
		int newVariance = (averageRange - newRange);
		if (newVariance < 0){ newVariance *= -1; }

		averageVariance = ((averageVariance + newVariance) / 2);
		averageRange = ((averageRange + newRange) / 2);

		Log.Debug(P("Ultrasonic Calibration: Range - %d, Variance - %d"), int(averageRange), averageVariance);

		baselineReads++;
		delay(BASELINE_READ_INTERVAL);
	}

	return averageRange;
}

/**
* Stop reading the ultrasonic range data and disable the sensor
*/
void stopRangefinder(){
	disableRangefinder();
	timer.deleteTimer(rangeTimerID);
}

/**
* Allow the ultrasonic rangefinder to send ping signals
*/
void enableRangefinder(){
	pinMode(RANGEFINDER_CONTROL_PIN, OUTPUT);
	digitalWrite(RANGEFINDER_CONTROL_PIN, HIGH);
}

/**
* Stop sending range pings.
*/
void disableRangefinder(){
	digitalWrite(RANGEFINDER_CONTROL_PIN, LOW);
}

/**
* Get the range in cm from the ultrasonic rangefinder
*
* @return Target distance from sensor in cm
*/
int getRange(){
	int targetDistance = rangeSensor.getRange();
	Log.Debug(P("Ultrasonic Range: %d cm"), targetDistance);
	return targetDistance;
}

/**
* Check the ultrasonic range sensor to see if a traffic event has occurred.
*
* Traffic events are counted as a break in the sensor's 'view' of the ground.
* Any object between the sensor and the ground baseline will cause the sensor
* to register a shorter range than usual.
*/
void checkRange(){
	int lastRange = trafficEntry[UVD_RANGE];
	int newRange = getRange();

	trafficEntry[UVD_RANGE] = newRange;

	// Detection occurs when target breaks the LoS to the baseline
	if ((rangeBaseline - newRange) > RANGE_DETECT_THRESHOLD && (lastRange - newRange) > RANGE_DETECT_THRESHOLD){

		// Increase traffic count
		int trafficCount = trafficEntry[COUNT_UVD];
		trafficCount++;
		trafficEntry[COUNT_UVD] = trafficCount;

		Log.Debug(P("Traffic count - UVD: %d counts"), trafficCount);

		// Also send an XBee alert
		printData();
		beep(NUM_BEEPS_UVD);
	}
}

/**
* Reset the traffic count for the ultrasonic rangefinder
*/
void resetRangeCount(){
	trafficEntry[COUNT_UVD] = 0;
	Log.Info(P("Traffic count reset - UVD"));
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

	enableLidar();
	lidar.begin();

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
	int lastRange = trafficEntry[LIDAR_RANGE];
	int newRange = getLidarRange();

	trafficEntry[LIDAR_RANGE] = newRange;

	// Detection occurs when target breaks the LoS to the baseline
	if ((lidarBaselineRange - newRange) > RANGE_DETECT_THRESHOLD && (lastRange - newRange) > RANGE_DETECT_THRESHOLD){

		// Increase traffic count
		int trafficCount = trafficEntry[COUNT_LIDAR];
		trafficCount++;
		trafficEntry[COUNT_LIDAR] = trafficCount;

		Log.Debug(P("Traffic count - Lidar: %d counts"), trafficCount);

		printData();
		beep(NUM_BEEPS_LIDAR);
	}
}

/**
* Reset the traffic count for the lidar
*/
void resetLidarCount(){
	trafficEntry[COUNT_LIDAR] = 0;
} 


//////////////////////////////////////////////////////////////////////////
// Motion Detector

/**
* Start the PIR motion sensor for motion detections
*/
void startMotionDetector(){
	enableMotion();

	pinMode(PIR_MOTION_PIN, INPUT);

	Log.Debug(P("Calibrating motion sensor - wait %d ms"), MOTION_INITIALISATION_TIME);
	delay(MOTION_INITIALISATION_TIME);
	motionDetected = false;

	motionTimerID = timer.setInterval(CHECK_MOTION_INTERVAL, checkPirMotion);
	Log.Debug(P("Motion started"));
}

/**
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
void checkPirMotion(){

	// Check the sensor
	if (digitalRead(PIR_MOTION_PIN) == MOTION_DETECTED){

		motionDetected = true;
		Log.Info(P("Motion detected"));

		// Increment PIR count
		long motionCount = trafficEntry[COUNT_PIR];
		motionCount++;
		trafficEntry[COUNT_PIR] = motionCount;

		Log.Info(P("Traffic count - PIR: %l counts"), motionCount);

		printData();
		beep(NUM_BEEPS_PIR);

		// Enter the cooldown phase
		motionCoolDown();

	}
	else{
		// No alarm; is fine
		motionDetected = false;
	}


	trafficEntry[PIR_STATUS] = motionDetected;
}

/**
* Temporarily pause checking the PIR motion sensor
*/
void motionCoolDown(){
	timer.deleteTimer(motionTimerID);
	motionTimerID = -1;
	timer.setTimeout(MOTION_COOLDOWN, resumeMotionDetection);
}

/**
* Start polling the PIR motion sensor
*/
void resumeMotionDetection(){
	motionTimerID = timer.setInterval(CHECK_MOTION_INTERVAL, checkPirMotion);
}

/**
* Reset the number of motion detections
*/
void resetMotionCount(){
	trafficEntry[COUNT_PIR] = 0;
}

/**
* Turn the motion sensor on - in a non-sexy way
*/
void enableMotion(){
	pinMode(PIR_CONTROL_PIN, OUTPUT);
	digitalWrite(PIR_CONTROL_PIN, HIGH);
}

/**
* Turn the motion sensor off
*/
void disableMotion(){
	digitalWrite(PIR_CONTROL_PIN, LOW);
}


//////////////////////////////////////////////////////////////////////////
// Optical Flow

/**
* Start and initialise the optical flow sensor
* The connection is confirmed by polling the device's product code
*/
void startOpticalFlow(){
	flowSensor.init(true);

	Log.Debug(P("Starting optical flow sensor"));

	byte retries = 0;
	bool flowConnected;
	while (!flowConnected && retries < OPTICAL_FLOW_MAX_RETRIES){
		flowConnected = checkOpticalFlowConnection();
		Log.Debug(P("ADNS3080 connected: %T"), flowConnected);
		retries++;
	}

	if (flowConnected){
		Log.Debug(P("Optical flow connected. Configuring..."));
		trafficEntry[OF_CONNECTED] = true;

		flowSensor.set_frame_rate_auto(true);
		flowSensor.set_shutter_speed_auto(true);
		flowSensor.set_resolution(1600);
	}

	flowTimerID = timer.setInterval(CHECK_FLOW_INTERVAL, updateFlowMotion);
}

/**
* Stop monitoring the optical flow sensor
*/
void stopOpticalFlow(){
	timer.deleteTimer(flowTimerID);
	flowTimerID = -1;
}

/**
* Start monitoring the optical flow sensor
*/
void resumeOpticalFlow(){
	flowSensor.clear_motion();
	flowTimerID = timer.setInterval(CHECK_FLOW_INTERVAL, updateFlowMotion);
}

/**
* Check the optical flow sensor to see if motion has been detected
*/
void updateFlowMotion(){
	bool connected = checkOpticalFlowConnection();
	trafficEntry[OF_CONNECTED] = connected;

	if (connected){
		byte motionRegister = flowSensor.read_register(ADNS3080_MOTION);
		bool flowDetected = motionRegister & 0x80;

		trafficEntry[OF_STATUS] = flowDetected;

		// Motion detected - determine the direction
		if (flowDetected){
			int8_t of_dx = flowSensor.read_register(ADNS3080_DELTA_X);
			int8_t of_dy = flowSensor.read_register(ADNS3080_DELTA_Y);

			trafficEntry[OF_X_OFFSET] = of_dx;
			trafficEntry[OF_Y_OFFSET] = of_dy;

			int of_count = trafficEntry[COUNT_OF];
			of_count++;
			trafficEntry[COUNT_OF] = of_count;

			Log.Debug(P("Traffic Count - OF: %d counts"), of_count);
			opticalFlowCooldown();

			printData();
			beep(NUM_BEEPS_OF);
		}
	}

}

/**
* Check that the optical flow sensor is connected by polling the product code
*/
bool checkOpticalFlowConnection(){
	byte productID = flowSensor.read_register(ADNS3080_PRODUCT_ID);
	bool flowConnected = (productID == 0x17);

	return flowConnected;
}

/**
* Stop monitoring the optical flow sensor for a cooldown period.
* Meant to be used immediately following detection events to avoid
* multiple detections for the same event
*/
void opticalFlowCooldown(){
	stopOpticalFlow();
	timer.setTimeout(FLOW_COOLDOWN, resumeOpticalFlow);
}

/**
* Reset the traffic count for the optical flow sensor
*/
void resetFlowCount(){
	trafficEntry[COUNT_OF] = 0;
}


//////////////////////////////////////////////////////////////////////////
// Buzzer

/**
* Set up the buzzer for use
*/
void startBuzzer(){
	pinMode(BUZZER_PIN, OUTPUT);
	buzzerOff();
}

/**
* Beep the buzzer once.
* Uses a timeout event to turn the buzzer off
*/
void beep(){
	digitalWrite(BUZZER_PIN, HIGH);
	timer.setTimeout(BUZZER_BEEP_PERIOD, beepCheck);
}

/**
* Beep the buzzer multiple times.
* At low buzzer periods, multiple beeps sound more like a trill.
*/
void beep(int numBeeps){
	if (numBeeps > 0){
		beepCount = 1;
		maxBeeps = numBeeps;

		beep();
	}
}

/**
* Callback for beep - Check if the buzzer needs to beep again
*/
void beepCheck(){
	buzzerOff();

	if (beepCount < maxBeeps){
		beepCount++;
		timer.setTimeout(BUZZER_BEEP_QUIET_PERIOD, beep);
	}
}

/**
* Turn the buzzer off
*/
void buzzerOff(){
	digitalWrite(BUZZER_PIN, LOW);
}

