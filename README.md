# TrafficCounter
Non-invasive traffic counter testbed for vehicular and pedestrian traffic. 
The testbed uses the Arduino platform, combined with low-cost and commonly available off-the-shelf sensors for ubiquitous traffic counting and classification for use inside Smart LED street lights. 

# Folders
##root
Contains the working code of the testbed, which is currently operating on the [WBoard Pro](http://imall.iteadstudio.com/im141125005.html). Functionally, the WBoard Pro is just an Arduino Mega, so either board will do. Or just change the pin assignments; what do I care?

##CAD
Graphical plans and 3D print files for mounting brackets and other peculiarities. 

##Libraries
Arduino Code libraries. What were you expecting?

##Pre-deployment Checks
Test code to check that each of the traffic sensors are functioning and calibrated before a recorded test or session.

##Tests
Test reports, recorded data, and archived code of platform deployments.

##Traffic Sink
Contains a Python script for data ingestion and uploading to Thingspeak. Change the API key in traffic_count_settings.py to suit your needs. My data is messy enough without other people injecting their own data...




