__author__ = 'Leenix'

from sys import platform as _platform
from SinkNode.Uploader.ThingspeakUploader import *

logger_level = logging.DEBUG
file_logger_level = logging.INFO
logger_name = "trafficCount"
log_filename = "trafficCount.log"
log_format = "%(asctime)s - %(levelname)s - %(message)s"

# Reader Settings #########################################
SERIAL_BAUD = 57600

if _platform == "linux" or _platform == "linux2":
    SERIAL_PORT = "/dev/ttyATH0"
else:
    SERIAL_PORT = "COM3"

PACKET_START = '#'
PACKET_STOP = '$'


# Processor Settings ######################################

TRAFFIC_KEY_MAP = {

    "count_uvd": "field1",
    "uvd_range": "field2",
    "count_pir": "field3",
    "count_of": "field4",
    "count_lidar": "field5",
    "lidar_range": "field6",
}

TRAFFIC_CHANNEL_MAP = {
    "trafficCount": "KY7G0UVNHA25GQ73",
}



# Uploader Settings #######################################



