Traffic Counter Testbed - 4th Test

Intro
=====


Aims
----

1) Test the new Lidar sensor to see if it more stable and accurate compared to the ultrasonic rangefinder (historically)

2) Test the new narrow-beam ultrasonic sensor 

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

Lidar
 - Baseline range of 518cm to the ground
 - No problems, seemed to detect just fine with a hand 30cm away from the sensor

OF
 - Sensor is now auto-adjusting to changes in light
 - can clearly see hand in frame from 50cm away
 - Motion not registered by sensor, but can see image at least...

Sonar
 - Baseline range of 626cm from the ground
 - readings are very twitchy, causing constant false detections
 - Readings appear to be defaulting to 626, rather than a baseline
 - Sensor is now operating off 3V3, which may be affecting its range

PIR
- Seems to be operating fine
- Hand gives a positive detection from 30cm away
-

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




Lidar
-----------------------

- Detection circle on ground appears to be a zone of 4cm radius
- Circle is not directly underneath the sensor, appearing approx. 1m to the right when looking down onto the road
- With a theoretical FOV of 3°, the detection circle should be approx. 25cm wide. 
- The detection circle is about right




