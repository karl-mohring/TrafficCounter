#include <ADNS3080.h>
#include <SPI.h>
#include <Logging.h>
#include "config.h"

AP_OpticalFlow_ADNS3080 flowSensor(OF_NCS, 0);
//SoftSPI SPI(OF_MOSI, OF_MISO, OF_SCLK);

void setup()
{
	Log.Init(LOGGER_LEVEL, SERIAL_BAUD);
	Log.Info("Optical SPI sensor test starting");

	pinMode(OF_NCS, OUTPUT);
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

