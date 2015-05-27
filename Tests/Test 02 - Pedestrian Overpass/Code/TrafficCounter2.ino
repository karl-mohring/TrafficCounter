#include <XBee.h>
#include <ADNS3080.h>
#include <SPI\SPI.h>
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

// PIR motion
bool motionDetected;
long timeOfLastMotion;

// Optical Flow
AP_OpticalFlow_ADNS3080 flowSensor(OPTICAL_FLOW_CHIP_SELECT_PIN, 0);
int8_t flow_dx;
int8_t flow_dy;

// Timers
SimpleTimer timer;
int rangeTimerID;
int motionTimerID;
int flowTimerID;

// Data storage
JsonObject<10> trafficEntry;
StraightBuffer sendBuffer(SEND_BUFFER_SIZE);

// Serial commands
char _commandCache[COMMAND_CACHE_SIZE];
CommandHandler commandHandler(_commandCache, COMMAND_CACHE_SIZE);

long entryNumber;

// XBee
XBee xbee = XBee();
ZBTxRequest zbTx = ZBTxRequest(COORDINATOR_ADDRESS, sendBuffer.getBufferAddress(), SEND_BUFFER_SIZE);
XBeeResponse response = XBeeResponse();
ZBRxResponse zbRx = ZBRxResponse();
ZBTxStatusResponse txStatus = ZBTxStatusResponse();


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
* startSensors
*
* Enable and configure the sensor suite for reading
*/
void startSensors(){
	trafficEntry["id"] = "trafficCount";
	trafficEntry["version"] = TRAFFIC_COUNTER_VERSION;

	trafficEntry["count_uvd"] = 0;
	trafficEntry["range"] = 0;

	trafficEntry["count_pir"] = 0;
	trafficEntry["pir_status"] = false;

	trafficEntry["count_of"] = 0;
	trafficEntry["of_dx"] = 0;
	trafficEntry["of_dy"] = 0;
	trafficEntry["of_status"] = false;

	startRangefinder();
	startMotionDetector();
	startOpticalFlow();
	}

/**
* CheckSerial
* 
* Check serial comms for incoming commands
*/
void checkSerial(){
	while (Serial.available()){
		char inChar = Serial.read();
		commandHandler.readIn(inChar);
	}
}

/**
* addSerialCommands
*
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
* defaultHandler
*
* Bad command received. Make fun of the sender
*/
void defaultHandler(char c){
	Log.Error(P("Command not recognised: %s"), c);
}

void printData(){
	Serial.print("#");
	trafficEntry.printTo(Serial);
	Serial.println("$");
}



//////////////////////////////////////////////////////////////////////////
// XBee

/**
* Set up the XBee for data transmission
*/
void startXbee(){
	Serial3.begin(57600);
	xbee.setSerial(Serial3);

	//transmitTimerID = timer.setInterval(XBEE_TRANSMIT_INTERVAL, sendXbeePacket);
}

/**
* Transmit the latest traffic entry over XBee
*/
void sendXbeePacket(){
	prepareXbeePacket();
	transmitData();
}

/**
* Package the latest traffic entry for sending over XBee
*/
void prepareXbeePacket(){
	sendBuffer.reset();

	sendBuffer.write('R');
	sendBuffer.write(':');
	sendBuffer.writeInt(trafficEntry["count_uvd"]);
	sendBuffer.write(',');

	sendBuffer.write('r');
	sendBuffer.write(':');
	sendBuffer.writeInt(int(trafficEntry["range"]));
	sendBuffer.write(',');

	sendBuffer.write('M');
	sendBuffer.write(':');
	sendBuffer.writeInt(int(trafficEntry["count_pir"]));
	sendBuffer.write(',');

	sendBuffer.write('m');
	sendBuffer.write(':');
	sendBuffer.write(int(trafficEntry["pir_status"]));
}

/**
* Send the recorded packet over the XBee
* Uses API mode transmission
*/
void transmitData(){
	zbTx = ZBTxRequest(COORDINATOR_ADDRESS, COORDINATOR_SHORT_ADDRESS, 0, EXTENDED_TIMEOUT,
		sendBuffer.getBufferAddress(), sendBuffer.getWritePosition(), 48);
	int retries = 0;
	bool packetSent = false;

	// Attempt to send until the packet transmits or times out
	while (!packetSent && retries <= XBEE_MAX_RETRIES){
		retries++;
	
		xbee.send(zbTx);

		// Check for acknowledgment
		if (xbee.readPacket(XBEE_ACK_TIMEOUT)) {
			Log.Debug(P("Response received - API [%i]"), xbee.getResponse().getApiId());

			// should be a znet tx status
			if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
				xbee.getResponse().getZBTxStatusResponse(txStatus);

				// get the delivery status, the fifth byte
				if (txStatus.getDeliveryStatus() == SUCCESS) {
					Log.Info(P("Packet delivery successful"));
					packetSent = true;

				}
				else {
					Log.Error(P("Packet delivery unsuccessful - [%i]"), txStatus.getDeliveryStatus());
				}
			}
		}

		else if (xbee.getResponse().isError()) {
			Log.Error(P("Could not receive packet"));
		}

		else {
			// Local XBee did not return a response - Happens when serial is in use
			Log.Error(P("No response from XBee (Serial in use)"));
		}
	}
}


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
	trafficEntry["range"] = averageRange;
	printData();

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
	int lastRange = trafficEntry["range"];
	int newRange = getRange();

	trafficEntry["range"] = newRange;
	
	// Detection occurs when target breaks the LoS to the baseline
	if ((rangeBaseline - newRange) > RANGE_DETECT_THRESHOLD && (lastRange - newRange) > RANGE_DETECT_THRESHOLD){

		// Increase traffic count
		int trafficCount = trafficEntry["count_uvd"];
		trafficCount++;
		trafficEntry["count_uvd"] = trafficCount;

		Log.Debug(P("Traffic count - UVD: %d counts"), trafficCount);

		// Also send an XBee alert
		printData();
	}

	
}

/**
* resetRangeCount
*
* Reset the traffic count for the ultrasonic rangefinder
*/
void resetRangeCount(){
	trafficEntry["count_uvd"] = 0;
	Log.Info(P("Traffic count reset - UVD"));
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
	// Only check the motion alarm if the cool-off has been exceeded
	if ((millis() - timeOfLastMotion) > (MOTION_TIMEOUT)){

		// Check the sensor
		if (digitalRead(PIR_MOTION_PIN) == MOTION_DETECTED){
			motionDetected = true;
			timeOfLastMotion = millis();

			Log.Info(P("Motion detected"));
			long motionCount = trafficEntry["count_pir"];
			motionCount++;
			trafficEntry["count_pir"] = motionCount;

			Log.Info(P("Traffic count - PIR: %l counts"), motionCount);
			printData();

		}
		else{
			// No alarm; is fine
			motionDetected = false;
		}
	}

	trafficEntry["pir_status"] = motionDetected;
}

/**
* resetMotionCount
*
* Reset the number of motion detections
*/
void resetMotionCount(){
	trafficEntry["count_pir"] = 0;
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
* disableMotion
*
* Turn off the motion sensor
*/
void disableMotion(){
	digitalWrite(PIR_CONTROL_PIN, LOW);
}


//////////////////////////////////////////////////////////////////////////
// Optical Flow

void startOpticalFlow(){
	flowSensor.init(true);

	Log.Debug(P("Starting optical flow sensor"));

	byte retries = 0;
	bool flowConnected;
	while (!flowConnected && retries < OPTICAL_FLOW_MAX_RETRIES){
		byte productID = flowSensor.read_register(ADNS3080_PRODUCT_ID);
		flowConnected = (productID == 0x17);
		Log.Debug(P("ADNS3080 Product ID: %X"), productID);
		retries++;
	}

	if (flowConnected){
		Log.Debug(P("Optical flow connected"));
	}

	flowTimerID = timer.setInterval(CHECK_FLOW_INTERVAL, updateFlowMotion);

}

void stopOpticalFlow(){
	timer.deleteTimer(flowTimerID);
	flowTimerID = -1;
}


void resumeOpticalFlow(){
	flowSensor.clear_motion();
	flowTimerID = timer.setInterval(CHECK_FLOW_INTERVAL, updateFlowMotion);
}


void updateFlowMotion(){
	byte motionRegister = flowSensor.read_register(ADNS3080_MOTION);
	bool motionDetected = motionRegister & 0x80;

	trafficEntry["of_status"] = motionDetected;

	if (motionDetected){
		int8_t of_dx = flowSensor.read_register(ADNS3080_DELTA_X);
		int8_t of_dy = flowSensor.read_register(ADNS3080_DELTA_Y);

		trafficEntry["of_dx"] = of_dx;
		trafficEntry["of_dy"] = of_dy;

		int of_count = trafficEntry["count_of"];
		of_count++;
		trafficEntry["count_of"] = of_count;

		Log.Debug(P("Traffic Count - OF: %d counts"), of_count);
		opticalFlowCooldown();

		printData();
	}
}

void opticalFlowCooldown(){
	stopOpticalFlow();
	timer.setTimeout(FLOW_COOLDOWN, resumeOpticalFlow);
}



void resetFlowCount(){
	trafficEntry["of_count"] = 0;

}
