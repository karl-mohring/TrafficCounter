#include <ADNS3080.h>
#include <SPI.h>
#include <Logging.h>
#include "config.h"

AP_OpticalFlow_ADNS3080 flowSensor(OPTICAL_FLOW_CHIP_SELECT_PIN, 0);
//SoftSPI SPI(OF_MOSI, OF_MISO, OF_SCLK);

void setup()
{
	startYunSerial();
	Log.Info("Optical SPI sensor test starting");

	pinMode(OPTICAL_FLOW_CHIP_SELECT_PIN, OUTPUT);
	pinMode(MISO, INPUT);
	pinMode(MOSI, OUTPUT);
	pinMode(SCK, OUTPUT);

	flowSensor.init();


	byte productID = flowSensor.read_register(ADNS3080_PRODUCT_ID);
	Log.Info("Product ID:  %X", productID);

	byte inverse_productID = flowSensor.read_register(ADNS3080_INVERSE_PRODUCT_ID);
	Log.Info("*Product ID: %X", inverse_productID);

	flowSensor.set_resolution(400);
	flowSensor.set_frame_rate_auto(false);
	flowSensor.set_shutter_speed_auto(false);
}

void loop()
{
	byte motion = flowSensor.read_register(ADNS3080_MOTION);
	bool motionDetected = motion & 0x80;
	byte surface_quality = flowSensor.read_register(ADNS3080_SQUAL);

	Log.Info("Motion: %T\t Surface quality: %d", motionDetected, surface_quality);

	if (motionDetected){
		int8_t dx = flowSensor.read_register(ADNS3080_DELTA_X);
		int8_t dy = flowSensor.read_register(ADNS3080_DELTA_Y);
		Log.Info("X: %d\nY: %d", dx, dy);
	}

	delay(500);
}

bool linuxBusy;

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


