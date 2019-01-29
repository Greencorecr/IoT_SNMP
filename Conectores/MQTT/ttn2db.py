# https://www.thethingsnetwork.org/docs/applications/python/

import time
import ttn
from tinydb import TinyDB, Query
db = TinyDB('/opt/UNA/MQTT/sensores.json')
Sensores = Query()

      
app_id = "sensores-demo" # En Overview, Application ID
access_key = "ttn-account-v2..." # Access Keys

def uplink_callback(msg, client):
  payload = msg.payload_fields
  metadata = msg.metadata
  gateways = metadata.gateways
  gwid = gateways[0].gtw_id
  # Debugs
  #print(gwid)
  #print(msg)
  #print(payload, metadata)
#----------------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------------
  # Valida que solo reciba datos del gateway gateway-demo 
  if gwid == 'gateway-demo':
      if payload.tipo == 'temphum':
          print ('--------------------------------------')
          print('HUMEDAD')
          print ('--------------------------------------')
          print(msg.dev_id, payload.temp, payload.hum, metadata.time)
          db.upsert({'dev_id': msg.dev_id, 'time': metadata.time, 'temp': payload.temp, 'hum': payload.hum}, Sensores.dev_id == msg.dev_id)
  
      #----------------------------------------------------------------------------------------------------
      #----------------------------------------------------------------------------------------------------
      elif payload.tipo == 'puerta':
          print ('--------------------------------------')
          print('PUERTA')
          print ('--------------------------------------')
         
          search = db.get(Sensores.dev_id == 'sensor-02-puerta')
          anterior = search.get("triggs_anterior")
          actual = payload.triggs
  
          # Debugs
          print(anterior)
          print(actual)
          print('---')
  
          #Se reseteo el sensor, inicia en 0
          if actual == 0:
              #print('if 0')
              #print(anterior)
              #print(actual)
              estado = 0
              db.upsert({'dev_id': msg.dev_id, 'time': metadata.time, 'estado': 0, 'triggs_anterior': 0, 'triggs': 0}, Sensores.dev_id == msg.dev_id)
  
          #No ha variado el dato, la puerta continua cerrada
          elif actual == anterior:
              #print('if ==')
              #print(anterior)
              #print(actual)
              estado = 0
              db.upsert({'dev_id': msg.dev_id, 'time': metadata.time, 'estado': estado, 'triggs_anterior': anterior, 'triggs': actual}, Sensores.dev_id == msg.dev_id)
  
          # Aumento el conteo, la puerta se abrio
          elif actual > anterior:
              #print('if >')
              #print(anterior)
              #print(actual)
              estado = 1
              db.upsert({'dev_id': msg.dev_id, 'time': metadata.time, 'estado': estado, 'triggs_anterior': actual, 'triggs': actual}, Sensores.dev_id == msg.dev_id)
              print(msg.dev_id, anterior, actual, estado, metadata.time)
  
          # Si el valor anterior es mayor que el actual, significa que la base de datos se desactualizó, se fuerza sincronizacion.
          else:
              #print('if <')
              #print(actual)
              #print(anterior)
              estado = 1
              db.upsert({'dev_id': msg.dev_id, 'time': metadata.time, 'estado': estado, 'triggs_anterior': actual, 'triggs': actual}, Sensores.dev_id == msg.dev_id)
  
          print(msg.dev_id, anterior, actual, estado, metadata.time)
  
      #----------------------------------------------------------------------------------------------------
      #----------------------------------------------------------------------------------------------------
      elif payload.tipo == 'gotas':
          print ('--------------------------------------')
          print('GOTAS')
          print ('--------------------------------------')
          print(msg.dev_id, payload.gotas, payload.triggs, metadata.time)
          db.upsert({'dev_id': msg.dev_id, 'time': metadata.time, 'gotas': payload.gotas, 'triggs': payload.triggs}, Sensores.dev_id == msg.dev_id)
      #----------------------------------------------------------------------------------------------------
      #----------------------------------------------------------------------------------------------------
      elif payload.tipo == 'consumo':
          print ('--------------------------------------')
          print('CONSUMO')
          print ('--------------------------------------')
          #print(msg.dev_id, payload.gotas, payload.triggs, metadata.time)
          #db.upsert({'dev_id': msg.dev_id, 'time': metadata.time, 'gotas': payload.gotas, 'triggs': payload.triggs}, Sensores.dev_id == msg.dev_id)
      #----------------------------------------------------------------------------------------------------
      #----------------------------------------------------------------------------------------------------
      elif payload.tipo == 'caida':
          print ('--------------------------------------')
          print('CAIDA')
          print ('--------------------------------------')
          print(msg.dev_id, payload.caida, metadata.time)
          db.upsert({'dev_id': msg.dev_id, 'time': metadata.time, 'caida': payload.caida}, Sensores.dev_id == msg.dev_id)
      #----------------------------------------------------------------------------------------------------
      #----------------------------------------------------------------------------------------------------
      else:
          print('Tipo no encontrado')
          print(payload.tipo)
  else:
      print('Paquete ignorado, gateway incorrecto')

handler = ttn.HandlerClient(app_id, access_key)

mqtt_client = handler.data()
mqtt_client.set_uplink_callback(uplink_callback)
mqtt_client.connect()
while True:
    time.sleep(60)
