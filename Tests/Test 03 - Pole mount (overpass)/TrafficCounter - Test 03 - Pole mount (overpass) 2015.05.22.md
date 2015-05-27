Traffic Counter Testbed - 3rd Test (Pole mounting)

Intro
=====

The third traffic counter test changed the mounting situation for the sensors from a shitty box to an outreach pole, not unlike those used by street light poles. A difference is that street light outreach poles aim to position the lamp closer to the centre of the roadway, while the sensor outreach pole is used to separate the traffic sensors away from any obstructions. The goals are different, but outreach poles help everyone.


Aims
----

1) Determine whether the outreach pole has fixed issues with the ultrasonic sensor
2) Run a test to ensure the optical flow sensor is actually working...
3) Test out the effectiveness of the new and adjustable PIR motion sensor


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
- Sensors were mounted to the underside of an aluminium outreach pole approximately 50cm long
- The pole was angled so the sensors would be facing straight down into the roadway
- The pole was secured at foot-level by a clamp and rope stay lines
- A nearby laptop was connected via USB to provide power to the sensors and also gather data for the duration of the test


Pre-Flight Checks
-----------------

UVD - The sensor output was checked for range to ensure the beam was not reflecting off the overpass or nearby surfaces. A manual reading was triggered by blocking the sensor with a hand

PIR - Similar to the ultrasonic sensor, the PIR sensor was manually triggered by moving a hand in front of the sensor. The PIR test does not have a cooldown timer, unlike the traffic counter firmware. Detection times and information is available in the test console.

Optical flow - To first calibrate the sensor, the RAW pixel data from the sensor is read and displayed on the laptop to ensure the frame is in focus. After calibration, the sensor's motion register is polled to check if motion has been detected by the sensor.



Procedure
---------

1) The sensor pole was secured to the outside railing of the pedestrian overpass
2) Power was supplied to the key chain camera using the laptop's USB port, and recording started well in advance of the sensor platform being turned on
3) Sensors were powered on and checked for reasonable input
4) Any necessary adjustments were applied before starting the datalogger script
- The optical flow sensor wiring became loose while securing the sensor platform to the railing and caused data to stop being transmitted by the sensor
    + The loose wire was secured at a fixed angle, which fixed the issue
- Took the lens cap off the optical flow sensor...
- The ultrasonic rangefinder was angled at various degrees to confirm readings were updating

5) Data was recorded and uploaded to Thingspeak using the datalogger script on the laptop


Data Analysis
-------------

1) Video data was reviewed to gain a comprehensive list of traffic information, including event times, traffic type (pedestrian, cyclist, vehicle), and whether or not a multiple event is occurring.
2) Count data was compared graphically between the actual video count and the other sensors, specifically:
    - PIR vs All traffic events (events; multiple events counted as one)
    - UVD vs Actual Vehicle traffic
    - New PIR events vs all traffic events (run-on events discarded)
 

Results & Analysis
==================

Over the course of the test, the video camera recorded a total of 50 combined traffic events (5 vehicles, 47 pedestrians, 5 cyclists, 1 scrub turkey). 

While the camera was focussed on the main detection area, the mounting and lens characteristics may mean that some events picked up the PIR sensor may not have been 'seen' by the camera.


UVD (and pole mount)
--------------------

Both during the pre-deployment check, and throughout the test, the ultrasonic rangefinder registered a baseline target distance of 792cm. While the range is likely not accurate (and beyond the sensors maximum rated range of 745cm), the ping does not appear to be reflecting off of the overpass edge. Range readings were less consistent in this test compared to previous deployments, as several pings were not returned, causing the range to read as 1024cm. 

During the pre-deployment check, several pedestrians walked underneath the sensor, but were not detected. However, a car driving on the road below was able to cause a detection to occur, but only on the incoming direction. The sensor did not detect the same car travelling in the opposite direction.

The ultrasonic sensor was not able to detect any pedestrian or cyclist traffic events during the recorded test, and only seemed to trigger when cars were on the road. Counting only vehicles, the UVD appeared to detect 3 of 5 vehicle events (not including the detection in the pre-deployment check) when compared to the video footage. The failed detection may be due to the sensor field of view looking at the footpath-road junction, rather than direction down onto the road, meaning the missing traffic events may have just been out of view of the sensor. 

The three positive vehicle detections by the ultrasonic rangefinder appear to occur a consistent 40 seconds before a vehicle detection event from the video, meaning there is likely a 40 second clock offset between the video camera, and the recording laptop.

1st vehicle - Not detected - Car was on the edge of the camera range, likely outside of the ultrasonic sensor detection area, cannot see roof in video

2nd vehicle - Detected - JCU buggy - reasonably close to the curb, large flat and high roof would assist detection

3rd vehicle - Detected - Sedan - reasonably close to curb, about the same as the buggy - hard to tell actual distances due to lens distortion

4th vehicle - Not detected - Far away from curb, cannot see roof in the video

5th vehicle - Detected - roof clearly visible from video, very close to the curb 

Vehicle detections caused the sensor to read range values from 532cm to 644cm, meaning car heights would have been 257cm and 144cm respectively. A vehicle height of 257cm is not entirely unreasonable for a 4wd or otherwise large vehicle, but only small sedans were observed during the test. The 144cm height is perfectly reasonable for the small cars observed during the test, which would indicate that cars were indeed being detected by the ultrasonic rangefinder. 


PIR
---

The PIR sensor performed as expected during the pre-deployment checks with the sensor firing only when a hand was moved in front of the sensor. Positive pedestrian detections occurred during the sensor check. The hardware motion cooldown reset after approximately 1.8 seconds. 

The software cooldown for the PIR sensor was set to reset after 2 seconds; not long after the hardware cooldown. The result of this low cooldown time is a large number of run-on events, where targets took longer than the cooldown time to cross the field of view, resulting in multiple traffic events being registered for the same target. Overall, 109 traffic events were captured by the PIR sensor, covering all types of traffic (the sensor is unable to distinguish between traffic types). A significant amount of these events were run-on motion events, where the motion event is still running even after the cooldown period has elapsed. The occurrance of run-on events is not surprising, considering pedestrians can take over 6 seconds to walk across the detection area, giving plenty of time for the sensor to trigger multiple times and causing an inflated traffic count. A workaround to eliminate these duplicate events was to discard traffic events where the previous sensor state was high (indicating a running detection event). Comparison of only 'new' detection events and video events showed a very high correlation between count timelines, as well as a very close total count (54 for PIR vs 50 for video).

Detection counts between PIR and video have very similar counts and times. The counts seem to diverge at points where multiple traffic events occur at once, indicating the PIR sensor may still be triggering twice for multiple events.

Very close (but separate) traffic events can be counted with video processing that cannot be detected by the PIR sensor because of its cooldown period.

The PIR count was consistently higher than the number of video events (apart from the start of the test), but also consistently lower than the actual traffic count (total number of pedestrians, cyclists, and vehicles)

Optical Flow
------------
 
 During the calibration for the optical flow sensor, the entire raw pixel data showed an entirely white frame that would only change when all light into the lens was blocked. Even the smallest amount of light would cause the sensor to completely white out. Changing the focal point of the lens did not affect the white-out. As this sensor has been used for outdoor navigation on multicopters, the sensor is clearly capable of outdoor usage, meaning the white-balance issues must be solvable.

 Furthermore, the sensor test, which is triggered by moving the entire sensor arm up, did not produce any motion detections. 

 While the sensor registered as "connected" during the pre-deployment check, the connection status changed to "disconnected" for the entirety of the recording period. Further checks in the lab with the same code did not experience this same sensor drop out.


Conclusions & Recommendations
=============================

UVD (and pole mount)
--------------------

Moving to the pole-mounted sensor deployment helped greatly in setting up and packing away sensors quickly and easily, allowing for consistent angling and overhang distance away from the side of the overpass. More importantly, the 40cm pole allowed for enough clearance for the ultrasonic rangefinder. Both in tethered and untethered tests (pole held out over road manually), moving the rangefinder away from the wall provided consistently and much more accurate readings. 

One possible issue with the current deployment setup is that the overpass is not level, and actually slopes at a 5° angle, meaning ultrasonic pings directed down towards the road will not hit and reflect perpendicularly. 
>The mounting bracket should be altered to correct for this slight angle.

The ultrasonic sensor was not able to detect any pedestrians during this test, but all detections occurred when a vehicle was on the road below, indicating that the rangefinder is able to detect vehicular traffic. However, vehicle detections only registered on the sensor when the car was very close to the road, almost directly under the sensor. 
>As the PIR sensor can detect movement over a wide area, the traffic counter should be placed closer to the middle of the road way to allow the ultrasonic sensor to detect traffic more accurately, while still allowing the PIR sensor to detect pedestrian traffic on the footpath.

The nature of ultrasonic sensor make the detection areas fairly unclear. 
>A buzzer should be added to the testbed to allow for rapid feedback on each of the sensors. Exact detection bounds can then be identified so the detection areas can be physically mapped out on the ground below for each of the sensors, and also picked up by the video footage. 


PIR
---

At the lowest sensitivity and shortest hardware cooldown, the PIR sensor was able to detect traffic events of all types with a great degree of accuracy (8% overshoot). However, the short sensor cooldown meant that many traffic events were counted twice. 
>The PIR cooldown time should be changed to more closely resemble the time it takes for a pedestrian to cross the detection boundaries for more accurate traffic counts.

Alternatively, only new traffic events should be counted to eliminate possible run-on events.

Optical Flow
------------
>Look into the datasheet to see if there's a way to fix the white balance, as the sensor is not currently able to output any useful data.

The optical flow sensor is not suitable at detecting motion in a fixed scenario, as the sensor operates by calculating movement over the entirety of the image, rather than individual objects moving through the frame. However, the raw output of the sensor allows for video image processing techniques to be used at a very low computational cost. The 900 pixel count is unsuitable for facial identification (which is a plus in this scenario), but is capable if identifying multiple objects at once, and categorising different moving objects, and calculating their approximate speed.





