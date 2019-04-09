# Conector de TinyDB a SNMP
# Temperatura y Humedad

from datetime import datetime, timedelta
from influxdb import InfluxDBClient
from pysnmp.hlapi import *

# Config

# Snmp
snmpCommunity = 'greencore'
snmpHost = '10.42.22.182'
snmpOID = '1.3.6.1.4.1.5.1'

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

influxclient = InfluxDBClient(influxHost, influxPort, influxUser, influxPassword, influxDbname)
influxresult = influxclient.query(influxQuery)
result = list(influxresult.get_points())
try:
    hum = result[0]['last']
    timeTTN = datetime.strptime(result[0]['time'][0:10] + " " + result[0]['time'][11:19], "%Y-%m-%d %H:%M:%S")
except:
    timeTTN = 0
    hum = -1
    if __debug__:
        print("no existe en influx")

# Medimos el tiempo según TTN, el tiempo de hace 5 minutos. Si no hemos recibido datos, el sensor responde "0"
timeFail=datetime.utcnow()

print('humedad')
if (timeTTN in locals()) and (timeFail - timedelta(minutes=5) < timeTTN and hum in locals() ):
    print(hum)
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
            print("Error: " + errorIndication)
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
