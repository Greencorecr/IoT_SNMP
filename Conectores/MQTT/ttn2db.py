# https://www.thethingsnetwork.org/docs/applications/python/

import time
import ttn
from tinydb import TinyDB, Query
db = TinyDB('./sensores.json')
from tinydb import Query
Sensores = Query()

app_id = "fede2" # En Overview, Application ID
access_key = "ttn-account-v2.fEdE2" # Access Keys

def uplink_callback(msg, client):
  payload = msg.payload_fields
  metadata = msg.metadata
  # Debugs
  print(msg)
  #print(payload, metadata)
  print(msg.dev_id, payload.temp, payload.hum, metadata.time)
  if payload.tipo == 'temphum':
      db.upsert({'dev_id': msg.dev_id, 'time': metadata.time, 'temp': payload.temp, 'hum': payload.hum}, Sensores.dev_id == msg.dev_id)
  elif payload.tipo == 'gotas':
      db.upsert({'dev_id': msg.dev_id, 'time': metadata.time, 'gotas': payload.gotas, 'triggs': payload.triggs}, Sensores.dev_id == msg.dev_id)
  elif payload.tipo == 'puerta':
      db.upsert({'dev_id': msg.dev_id, 'time': metadata.time, 'triggs': payload.triggs}, Sensores.dev_id == msg.dev_id)


handler = ttn.HandlerClient(app_id, access_key)

mqtt_client = handler.data()
mqtt_client.set_uplink_callback(uplink_callback)
mqtt_client.connect()
while True:
    time.sleep(60)
