Intro
=====

The traffic counter test was used to test the accuracy of two sensors for non-invasive traffic detection. Ultrasonic vehicle detection uses the distance between the sensor and a distance target to determine whether or not traffic event is occurring. Normally, this distance will be from the sensor to the road surface if the sensor is deployed in an overhead position. Cars or pedestrians underneath will cause the sensor to register a shorter distance between the sensor and its target. This way, the sensor can determine the presence of an object in the beam path, and motion can be calculated using successive readings.

The second sensor uses Passive Infrared to monitor the surrounding environment and determine if any movement has occurred. All objects with temperatures above absolute zero radiate infrared energy that can be picked up the a sensor. The PIR sensor takes a snapshot of the surrounding area and checks for changes in infrared radiation. Any changes between snapshots indicate that something has moved.


Aim
---

1) Determine whether or not the ultrasonic rangefinder could be used to count pedestrian traffic when mounted at streetlight level
2) Determine whether or not the PIR sensor could be used to count pedestrian traffic when mounted at streetlight level


Methodology
===========


Sensors
-------

- Ultrasonic rangefinder (Maxbotix XL-Maxsonar-EZ2 1320)
- PIR motion sensor (DFRobotics PIR AM412)
- Keychain camera (808 #16)

Data recorder
-------------

- Both ultrasonic and PIR sensors were periodically sampled by the WBoard Pro (Mega 2560) at a rate of 5 Hz and 1 Hz respectively. The low sample rate with the PIR sensor is due to its long motion timeout of 3 seconds. That is, any movement picked up by the sensor would cause the device to advertise motion for 3 seconds.
- The keychain camera recorded constant video at a rate of 10 fps, with hard timestamps
- Traffic counts from each of the sensors was monitored and kept in local memory
- Data was written to an integrated SD card every 5 seconds
- The running sensor data was also transmitted to a nearby base station using an XBee transceiver
- The base station kept a log file of all received transmissions and uploaded the entries to a Thingspeak channel for monitoring over the web


Test location
-------------

- The sensor box was mounted on the roof of Building 17, on a pole overlooking the side entrance of the building
- Both sensors and the camera were pointed to capture the middle of the walkway (slightly off straight down)
- Power was supplied using a USB charger connected to a nearby power outlet on the roof of Building 17

Procedure
---------

1) The sensors and camera were secured at a fixed angle inside a container
2) The container was then secured to the mounting pole, then adjusted to achieve the desired angle
3) Power was switched on to the camera and microcontroller board to start sensor calibration and data recording
4) The nearby base station started receiving data soon afterwards and uploading the data online.

5) The sensors were left to record over the next 2 hours and 28 minutes
6) After the recording session, traffic events were mapped into time periods.
7) Actual pedestrian numbers were determined by reviewing the recorded video footage and mapped into the same time periods
8) Traffic counts, as well as event times were compared between the ultrasonic rangefinder, PIR sensor, and the video recording.


Results and Analysis
--------------------

Video
-----

Manually reviewing the recorded video footage revealed that a total of 80 people entered or exited the building over the couse of the test over 75 traffic events (some were double events). 
- Visual confirmation of traffic events was extremly valuable in gaining an accurate number of detection, as well as timestamps
- Video analysis also made double detections possible where other sensing methods may not perform as well
- The video was recorded to an onboard SD card, which allows the camera to be compact, but video can only be reviewed after recording has finished.
- Also, the video footage needed to be reviewed manually, which takes time and is prone to human error. Several video image processing programs exist for pedestrian and vehicle detection, but require more processing power that what is available in the test platform.

> Continue using manual video review for accurate traffic event coints



UVD
---

Over the course of the test, the ultrasonic sensor recorded 61 events. The ultrasonic and PIR traffic data did not have access to a real-time clock, meaning mapping needed to be approximated using base station timestamps and video events.
A direct comparison between the video counts and the ultrasonic detections shows a correlation between the two systems that diverges at the end. However, the correlation may be a result of erroneous range measurements.
According to the rangefinder, the sensor is located approximately 660 cm above the ground, which is not as high as expected. The beam may be reflecting off something on the wall.
- The ultrasonic rangefinder detected 61 individual traffic events, which is similar to the actual 75 events (61/75 events = 81.3% accuracy)
- While the count is similar, there is only a loose correlation between the video detection times and the ultrasonic detection times. Granted, there is an uncertainty in the timing offset between the video and the sensor data. Some ultrasonic detections appear entirely random, with no visible events that could trigger the sensor. Similarly, there are several traffic events that occur on video that are not detected by the ultrasonic sensor.
- At this point in time, there is no way of knowing whether the detections reported by the ultrasonic sensor are genuine or cause by erroneous readings, especially when the baseline height of the sensor is suspiciously low
- Another thing to consider is that the ultrasonic rangefinder was not inteded for pedestrian sensing, as the low cross-sectional area of pedestrians from an overhead perspective is low. Cars and other road vehicle exhibit a much larger cross-sectional area, which makes them more likely to be detected by the ultrasonic detector.

> Look for a better testing space for traffic testing


PIR
---

The PIR motion sensor detected 723 traffic events over the duration of the test, which was well in excess of the 75 actual events counted by the video footage. 
- While the sensor behaves as expected in an indoor lab condition, with motion alarms being triggered when objects are moving across the sensors field of view, the sensor behaves drastically different in outdoor environments. The sensitivity of the sensor caused the motion alarm to fire much more often. 
- One possibility is that small changes in lighting, such as those cast by shadows of surrounding vegetation, may have triggered the sensor to detect an event
- Another possible cause of the sensor's high detection rate is the altered lighting conditions compared to a lab environment. Direct sunlight is much brighter than indoor lighting. Perhaps there are peculiarities with this particular sensor and direct sunlight.

> Try the sensor again in a different location
> Find another PIR motion detector with an adustable sensitivity, so outdoor noise can be filtered
> Search the sensor's datasheet to see if there are and usage instructions for outdoor environments


Conclusions & Recommendations
=============================

Video
-----

Video monitoring proved to be extremely useful for counting traffic to verify the numbers presented by the other sensors. However, automating the footage review process is computationally intensive, and cannot be performed by the sensor platform hardware. Video image processing should still be used in the field of testing, but cannot be used for actual traffic sensing without using a much more powerful processor to handle the computational load.

UVD
---

The sensor output needs to be confirmed as correct through more testing.
Another testing location may be needed to better replicate the conditions found on streetlights with open spaces away from wall surfaces. 

PIR
---

Although the final traffic count registered b the motion sensor far exceeded the actual number of traffic events, motion sensing has been used by many pedestrian sensing effors in the past. Adjustable sensitivity, as well as appropriate lensing may be needed to handle the imitations that PIR sensors face when deployed in outdoor environments.


This test was the first outdoor deployment of the traffic sensor suite. The location, while not necessarily typical of a street light deployment, presented many challenges that would be present in an outdoor environment, including large nearby surfaces, and constantly changing environments.




