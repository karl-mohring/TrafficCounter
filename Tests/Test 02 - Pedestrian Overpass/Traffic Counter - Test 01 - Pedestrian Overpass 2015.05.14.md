Intro
=====

The second traffic counting test used various low-cost sensors to count both vehicular and pedestrian traffic in real time.

Aims
----
1) Test the ultrasonic rangefinder sensor in an open environment away from walls
2) Test the ultrasonic rangefinder with vehicular detection as well as pedestrian detection
3) Confirm or deny the functionality of optical flow sensing for traffic detection
4) Confirm that the PIR motion sensor was not suitable in outdoor environments

Methodology
===========

Sensors
-------

- Ultrasonic rangefinder (Maxbotix XL-Maxsonar-EZ2 1320)
- PIR motion sensor (DFRobotics PIR AM412)
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

Procedure
---------

1) The sensor container was secured to the outside railing of the pedestrian overpass
2) Power was supplied to the key chain camera using the laptop's USB port, and recording started well in advance of the sensor platform being turned on
3) Sensors were powered on and checked for reasonable input
4) Any necessary adjustments were applied before starting the datalogger script
- The optical flow sensor wiring became loose while securing the sensor platform to the railing and caused data to stop being transmitted by the sensor
    + The loose wire was secured at a fixed angle, which fixed the issue
- Took the lens cap off the optical flow sensor...
- The ultrasonic rangefinder was angled at various degrees to confirm readings were updating

5) Data was recorded and uploaded to Thingspeak using the datalogger script on the laptop
6) Recorded data was reviewed at the end of the test, with the option of using the recorded video feed to confirm numbers
 
Results & Analysis
==================

- Start time - 13:39:43
- Finish time- 15:14:21

- Running time - 1 hour 35 minutes


UVD
---

The ultrasonic sensor did not detect any pedestrians or vehicles at all during the test, and consistently returned a target distance between 60 cm and 90 cm. Despite a large volume of people walking underneath the sensor during the test, no events were registered by the sensor.

- The height of the pedestrian overpass is approximately 4 m above the road surface, meaning the sensor readings are clearly not accurate.
- The ultrasonic ping may be reflecting off the side of the overpass and giving false readings


PIR
---

Unlike the other sensors, the motion sensor recording an extremely high number of traffic events (1103 in total). The sensor was detecting movement when no pedestrians or vehicles were on the road.

- The sensor is much too sensitive for outdoor environments, where small changes are constantly occurring 


Optical flow
------------

No traffic events were detected by the optical flow sensor.

- The loose wiring is a concern and a possible cause for the optical flow sensor not detecting any events
- Optical flow operates on the basis that the entire frame moves a fixed amount. A single object moving across a fixed frame may not trigger the motion detection algorithm


Conclusions & Recommendations
=============================

UVD
---

The ultrasonic rangefinder is giving false readings, likely as a result of the ultrasonic ping reflecting off the bridge prematurely, instead of reflecting off the road surface. 
- A rangefinder with a narrow beam width (EZ4) may be used to replace the current sensor (that uses a medium beam width EZ2). 
- A second recommendation is to physically move the ultrasonic sensor further out and away from the edge of the overpass to avoid any reflected signals

> Order a Maxbotix XL-Maxsonar-EZ4 for further testing
> Determine the beam width of the ping on the ground for the next test
> Build a mounting arm for further tests on the overpass


PIR
---

For the second time, the motion sensor has given readings far higher than the number of actual traffic events. Motion detection is still a feasible method of traffic detection, but the sensor used in this test is far too sensitive to small changes in the environment to be of any use.
- Even in indoor environments, very small movements cause detection events.
- This particular PIR sensor does not have a configurable sensitivity. Replace the sensor with something that can be configured.

> Order a SE-10 or other PIR with configurable sensitivity and hardware timeout
> The sensor will need to be calibrated for outdoor environments


Optical Flow
------------

No traffic events were recorded by the sensor, despite having a clear view of the street below in full daylight. Connection problems may have caused the sensor to become unresponsive. Otherwise, the type of sensing performed by the sensor may not be suited to tracking the movement of individual objects.
The individual pixels of the sensor can be extracted to perform analysis on the raw data, but that would take more time than we have.
- Perform a connection check each time the sensor is read to ensure the reading is valid or if problems are occurring 
- The sensor should be re-calibrated and re-tested to ensure sensing conditions are fine

> Add code to check connectivity of the OF sensor - reflect disconnects in the data
> Perform some individual tests using the OF sensor


Further Recommendations
-----------------------

Given that no sensor has performed particularly well in the two traffic tests, it may be time to test another sensor type. Sparkfun's LIDAR lite uses laser ranging up to 40 m away. 

> Order a LIDAR from sparkfun





