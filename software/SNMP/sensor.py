# Conector de InfluxDB y SNMP en ESP32 a SNMP unificado
# Realiza un failover entre datos insertados a InfluxDB, y datos en tiempo
# real consultados a sensores ESP32 vía SNMP. Sin importar la fuente de los
# datos, se empaquetan en una respuesta de SNMP para ser consumidos por
# software de monitoreo tipo NMS

# Es genérico para cada tipo de sensor, donde solamente configuramos el sensor
# a consultar, el tipo de respuesta a enviar, y la consulta en la BD. Editar:
# - snmpHost y snmpOID para reflejar los datos del sensor ESP32
# - snmpHeader para el tipo de respuesta a crear, 'temperatura', 'amp1', etc
# - influxQuery definiendo LAST(Sensor) según 'fields' y el nombre de la tabla
#   que contiene los datos según 'measurement' en el script ../MQTT/ttn2db.py

from datetime import datetime, timedelta
from influxdb import InfluxDBClient
from pysnmp.hlapi import *

# Config

# Consulta SNMP a ESP32
snmpCommunity = 'greencore'
snmpHost = '10.42.22.182'
snmpOID = '1.3.6.1.4.1.5.1'

# Respuesta SNMP que construimos
snmpHeader = 'humedad'

# InfluxDB
influxHost='localhost'
influxPort=8086
influxUser = 'root'
influxPassword = 'root'
influxDbname = 'sensores'
influxDbuser = 'greencore'
influxDbuser_password = 'my_secret_password'
influxQuery = 'select LAST(Hum) FROM sensortemphum;'

# Fin Config

influxClient = InfluxDBClient(influxHost, influxPort, influxUser, influxPassword, influxDbname)
influxResult = influxClient.query(influxQuery)
result = list(influxResult.get_points())
try:
    sensorData = result[0]['last']
    timeTTN = datetime.strptime(result[0]['time'][0:10] + " " + result[0]['time'][11:19], "%Y-%m-%d %H:%M:%S")
except:
    timeTTN = 0
    sensorData = -1
    if __debug__:
        print("no existe en influx")

# Medimos el tiempo según TTN, el tiempo de hace 5 minutos. Si no hemos recibido datos, el sensor responde "0"
timeFail=datetime.utcnow()

# Se inicia la creación de la respuesta de SNMP
print(snmpHeader)
if (timeTTN in locals()) and (timeFail - timedelta(minutes=5) < timeTTN and sensorData in locals() ):
    print(sensorData)
    if __debug__:
        print("TTN")
else:
    try:
        errorIndication, errorStatus, errorIndex, varBinds = next(
           getCmd(SnmpEngine(),
           CommunityData(snmpCommunity, mpModel=0),
           UdpTransportTarget((snmpHost, 161)),
           ContextData(),
           ObjectType(ObjectIdentity(snmpOID)))
        )
        if errorIndication:
            print("Error: " + str(errorIndication))
        elif errorStatus:
            print('%s at %s' % (errorStatus.prettyPrint(),
                                errorIndex and varBinds[int(errorIndex) - 1][0] or '?'))
        else:
            snmpdata = str(varBinds[0]).split("= ")
            print(snmpdata[1])
            if __debug__:
                print("snmp")
    except:
        print("-1")
