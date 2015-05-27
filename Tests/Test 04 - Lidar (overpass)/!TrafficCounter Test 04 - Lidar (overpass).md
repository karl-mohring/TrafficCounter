Traffic Counter Testbed - 4th Test

Intro
=====


Aims
----


Methodology
===========

Sensors
-------

- Ultrasonic rangefinder (Maxbotix XL-Maxsonar-AE2 1320)
- PIR motion sensor (HC-SR501)
- Optical flow sensor (ADNS3080)
- Keychain camera (808 #16)

Data recorder
-------------

- The ultrasonic, optical flow, and PIR sensors are periodically sampled by the WBoard Pro (Mega 2560) at a rate of 10Hz, 5Hz, and 1Hz respectively
- The keychain camera recorded constant video at a rate of 10 fps, with hard timestamps
- Whenever a traffic event is detected, all tracked data, including traffic counts from each of the sensors, sensor detection states, and other information is packaged and transferred across the USB serial link to the laptop, which is running a datalogger script
- The datalogger script adds a timestamp to each received entry and transfers the data into a log file
- Data entries are also uploaded to the Thingspeak service for real-time data monitoring over the web
- No local data storage occurs on the microcontroller board

Test location
-------------

- The second traffic sensing test was located at the pedestrian overpass outside building 34
- Sensors were secured inside a plastic container, which was then fixed to the railing directly above divide between the pedestrian footpath and roadway
- The container was angled so the sensors would be facing straight down into the roadway
- The container was secured at foot-level
- A nearby laptop was connected via USB to provide power to the sensors and also gather data for the duration of the test


Pre-Flight Checks
-----------------



Test Procedure
---------

1) The sensor container was secured to the outside railing of the pedestrian overpass
2) Power was supplied to the key chain camera using the laptop's USB port, and recording started well in advance of the sensor platform being turned on
3) Sensors were powered on and checked for reasonable input
4) Any necessary adjustments were applied before starting the datalogger script
- The optical flow sensor wiring became loose while securing the sensor platform to the railing and caused data to stop being transmitted by the sensor
    + The loose wire was secured at a fixed angle, which fixed the issue
- Took the lens cap off the optical flow sensor...
- The ultrasonic rangefinder was angled at various degrees to confirm readings were updating

Data Analysis
-------------


 
Results & Analysis
==================



Conclusions & Recommendations
=============================

UVD
---



PIR
---



Optical Flow
------------




Further Recommendations
-----------------------





