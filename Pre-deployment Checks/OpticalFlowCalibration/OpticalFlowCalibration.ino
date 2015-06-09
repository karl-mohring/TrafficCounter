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

	flowSensor.set_frame_rate_auto(true);
	flowSensor.set_shutter_speed_auto(true);
	flowSensor.set_resolution(1600);

	byte productID = flowSensor.read_register(ADNS3080_PRODUCT_ID);
	Log.Info("Product ID:  %X", productID);

	byte inverse_productID = flowSensor.read_register(ADNS3080_INVERSE_PRODUCT_ID);
	Log.Info("*Product ID: %X", inverse_productID);

}

void loop()
{

	Serial.println("image data --------------");
	flowSensor.print_pixel_data(&Serial);
	Serial.println("-------------------------");

}

