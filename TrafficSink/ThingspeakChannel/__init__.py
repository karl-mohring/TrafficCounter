__author__ = 'Leenix'

from thingspeak_config import *
import urllib
import httplib


class ThingspeakChannel(object):
    """
    Thingspeak upload manager based on the thingspeak library by bergey
    """
    HEADERS = {"Content-type": "application/x-www-form-urlencoded", "Accept": "text/plain"}

    @staticmethod
    def update(entry,server_address=SERVER_ADDRESS):
        params = urllib.urlencode(entry)
        conn = httplib.HTTPConnection(server_address)
        conn.request("POST", "/update", params, ThingspeakChannel.HEADERS)
        response = conn.getresponse()
        conn.close()
        return response

    @staticmethod
    def fetch(read_key, format_, server_address=SERVER_ADDRESS):
        conn = httplib.HTTPConnection(server_address)
        path = "/channels/{0}/feed.{1}".format(read_key, format_)
        params = urllib.urlencode([('key', read_key)])
        conn.request("GET", path, params, ThingspeakChannel.HEADERS)
        response = conn.getresponse()
        return response

    @staticmethod
    def map_entry(entry):
        """Process an incoming JSON entry into thingspeak format.

        Field mapping can be found in the settings.py file in the following format:
        date field name: thingspeak field name

        The CHANNEL_MAP list gives each ID the proper API key
        so the data is entered into the correct channel (assuming
        each unit has its own channel.

        :param entry: JSON format of sensor data = {
                        "id": unit_id,
                        "temperature": temp_data,
                        "humidity": humidity_data,...
                        }

        :return: JSON data in Thingspeak format = {
                        "key": API_KEY
                        "field1": field1_data
                        "field2": field2_data...
                        }
        """

        output = {}

        # Each entry must have an ID to be valid so we know where it's going
        if "id" in entry and entry["id"] in CHANNEL_MAP:
            channel_key = CHANNEL_MAP[entry["id"]]
            output["key"] = channel_key

            # Map the rest of the data into fields
            # Extra data will be ignored
            for k in entry:
                if k in KEY_MAP:
                    new_key = KEY_MAP[k]
                    output[new_key] = entry[k]

        return output
