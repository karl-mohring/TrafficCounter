#!/usr/bin/env python

from SinkNode.Processor.ThingspeakProcessor import ThingspeakProcessor
from SinkNode.Reader.SerialReader import SerialReader
from SinkNode import *
from traffic_count_settings import *


def start_logger():
    logger = logging.getLogger(logger_name)
    logger.setLevel(logger_level)

    # Console Logging
    console_handler = logging.StreamHandler()
    console_handler.setFormatter(logging.Formatter(log_format))
    logger.addHandler(console_handler)

    # File Logging
    if log_filename:

        file_handler = logging.FileHandler(log_filename)
        file_handler.setFormatter(logging.Formatter(log_format))
        file_handler.setLevel(file_logger_level)
        logger.addHandler(file_handler)


if __name__ == '__main__':
    start_logger()

    reader = SerialReader(SERIAL_BAUD,
                          SERIAL_PORT,
                          start_delimiter=PACKET_START,
                          stop_delimiter=PACKET_STOP,
                          logger_name=logger_name)

    processor = ThingspeakProcessor(key_map=TRAFFIC_KEY_MAP,
                                    channel_map=TRAFFIC_CHANNEL_MAP)

    uploader = ThingspeakUploader(logger_name=None)

    sink = SinkNode(reader, processor, uploader)
    sink.run()
