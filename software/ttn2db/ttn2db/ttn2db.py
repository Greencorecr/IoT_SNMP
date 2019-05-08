# https://www.thethingsnetwork.org/docs/applications/python/

import time
import ttn
from influxdb import InfluxDBClient

app_id = "desarrollo-otaa" # En Overview, Application ID
access_key = "ttn-account-v2.aqs55OCvGDocNr3z81LrbGcLh4rNteJuvQOuIM-Ehzk" # Access Keys

host='localhost'
port=8086
user = 'root'
password = 'root'
dbname = 'sensores'
dbuser = 'greencore'
dbuser_password = 'my_secret_password'
query = 'select Count from sensor;'
influxclient = InfluxDBClient(host, port, user, password, dbname)

def find_between_r( s, first, last ):
  try:
      start = s.rindex( first ) + len( first )
      end = s.rindex( last, start )
      return s[start:end]
  except ValueError:
      return ""

def uplink_callback(msg, client):
  payload = msg.payload_fields
  metadata = msg.metadata
  gateways = metadata.gateways
  rssi = gateways[0].rssi
  snr = gateways[0].snr
  channel = gateways[0].channel
  data_rate = metadata.data_rate
  SF = find_between_r(data_rate, "SF", "BW")

  if payload.tipo == 'amps':
      if __debug__:
          print('Consumo - SF:', SF, 'channel:', channel, 'rssi:', rssi, 'snr:', snr, 'dev_id:', msg.dev_id, 'time:', metadata.time, 'amp1:', payload.amp1, 'amp2:', payload.amp2, 'amp3:', payload.amp3, 'amp4:', payload.amp4)
      json_body = [
          {
              "measurement": "sensoramps",
              "tags": {
                  "dev_id": msg.dev_id,
                  "type": payload.tipo
              },
              "time": metadata.time[0:19] + "Z",
              "fields": {
                  "Amp1": float(payload.amp1),
                  "Amp2": float(payload.amp2),
                  "Amp3": float(payload.amp3),
                  "Amp4": float(payload.amp4),
                  "SF": int(SF),
                  "channel": channel,
                  "rssi": float(rssi),
                  "snr": float(snr),
              }
          }
      ]
      try:
          influxclient.write_points(json_body)
      except:
          print("Write points: {0}".format(json_body))
          print ("Error: paquete con error")
  elif payload.tipo == 'caida':
      json_body = [
          {
              "measurement": "sensorcaida",
              "tags": {
                  "dev_id": msg.dev_id,
                  "type": payload.tipo
              },
              "time": metadata.time[0:19] + "Z",
              "fields": {
                  "Caida": payload.caida,
                  "SF": int(SF),
                  "channel": channel,
                  "rssi": float(rssi),
                  "snr": float(snr),
              }
          }
      ]
      try:
          influxclient.write_points(json_body)
      except:
          print("Write points: {0}".format(json_body))
          print ("Error: paquete con error")
      if __debug__:
          print('Caida   - SF:', SF, 'channel:', channel, 'rssi:', rssi, 'snr:', snr, 'dev_id:', msg.dev_id, 'time:', metadata.time, 'caida:', payload.caida)
  elif payload.tipo == 'gotas':
      json_body = [
          {
              "measurement": "sensorgotas",
              "tags": {
                  "dev_id": msg.dev_id,
                  "type": payload.tipo
              },
              "time": metadata.time[0:19] + "Z",
              "fields": {
                  "Gotas": payload.gotas,
                  "SF": int(SF),
                  "channel": channel,
                  "rssi": float(rssi),
                  "snr": float(snr),
              }
          }
      ]
      try:
          influxclient.write_points(json_body)
      except:
          print("Write points: {0}".format(json_body))
          print ("Error: paquete con error")
      if __debug__:
          print('Gotas   - SF:', SF, 'channel:', channel, 'rssi:', rssi, 'snr:', snr, 'dev_id:', msg.dev_id, 'time:', metadata.time, 'gotas:', payload.gotas, 'triggs:', payload.triggs)
  elif payload.tipo == 'temphum':
      json_body = [
          {
              "measurement": "sensortemphum",
              "tags": {
                  "dev_id": msg.dev_id,
                  "type": payload.tipo
              },
              "time": metadata.time[0:19] + "Z",
              "fields": {
                  "Temp": float(payload.temp),
                  "Hum": float(payload.hum),
                  "SF": int(SF),
                  "channel": channel,
                  "rssi": float(rssi),
                  "snr": float(snr),
              }
          }
      ]
      try:
          influxclient.write_points(json_body)
      except:
          print("Write points: {0}".format(json_body))
          print ("Error: paquete con error")
      if __debug__:
          print('Temphum - SF:', SF, 'channel:', channel, 'rssi:', rssi, 'snr:', snr, 'dev_id:', msg.dev_id, 'time:', metadata.time, 'temp:', payload.temp, 'hum:', payload.hum)
  elif payload.tipo == 'puerta':
      json_body = [
          {
              "measurement": "sensorpuerta",
              "tags": {
                  "dev_id": msg.dev_id,
                  "type": payload.tipo
              },
              "time": metadata.time[0:19] + "Z",
              "fields": {
                  "Open": payload.Open,
                  "Count": payload.Count,
                  "SF": int(SF),
                  "channel": channel,
                  "rssi": float(rssi),
                  "snr": float(snr),
              }
          }
      ]
      try:
          influxclient.write_points(json_body)
      except:
          print("Write points: {0}".format(json_body))
          print ("Error: paquete con error")
      if __debug__:
          print('Puerta  - SF:', SF, 'channel:', channel, 'rssi:', rssi, 'snr:', snr, 'dev_id:', msg.dev_id, 'time:', metadata.time, 'Open:', payload.Open, 'Count:', payload.Count)
  else:
      if __debug__:
          print('Tipo no encontrado' + payload.tipo)



handler = ttn.HandlerClient(app_id, access_key)

mqtt_client = handler.data()
mqtt_client.set_uplink_callback(uplink_callback)
mqtt_client.connect()
while True:
    time.sleep(60)

