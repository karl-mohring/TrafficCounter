__author__ = 'Leenix'

# Mapping between data id keys and Thingspeak fields
KEY_MAP = {
    "uvd_count": "field1",
    "uvd_range": "field2",
    "pir_count": "field3",
    "pir_status": "field4",
}

# Mapping between unit ID and Thingspeak channel
CHANNEL_MAP = {
    "traffic": "KY7G0UVNHA25GQ73",
}

# Thingspeak server address (change if using a custom server)
# default: "api.thingspeak.com:80"
SERVER_ADDRESS = "api.thingspeak.com:80"
