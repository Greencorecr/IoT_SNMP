# Conector de TinyDB a SNMP
# Temperatura y Humedad

from datetime import datetime, timedelta
from influxdb import InfluxDBClient
from pysnmp.hlapi import *

errorIndication, errorStatus, errorIndex, varBinds = next(
    getCmd(SnmpEngine(),
           CommunityData('greencore', mpModel=0),
           UdpTransportTarget(('10.42.22.108', 161)),
           ContextData(),
           ObjectType(ObjectIdentity('1.3.6.1.4.1.5.0')))
)

def TTN2time(search):
    """
    Convierte el formato de hora de TTN, a algo usable con datetime
    """
    timeTTN = datetime.strptime((search.get("time")[0:10] + " " + search.get("time")[11:19]), "%Y-%m-%d %H:%M:%S")
    return timeTTN

# SELECT LAST("water_level") FROM "h2o_feet" WHERE "location" = 'santa_monica'
host='localhost'
port=8086
user = 'root'
password = 'root'
dbname = 'sensores'
dbuser = 'greencore'
dbuser_password = 'my_secret_password'
query = 'select LAST(Hum) FROM sensortemphum;'
influxclient = InfluxDBClient(host, port, user, password, dbname)
influxresult = influxclient.query(query)
result = list(influxresult.get_points())
hum = result[0]['last']
timeTTN = datetime.strptime(result[0]['time'][0:10] + " " + result[0]['time'][11:19], "%Y-%m-%d %H:%M:%S")

# Medimos el tiempo según TTN, el tiempo de hace 5 minutos. Si no hemos recibido datos, el sensor responde "0"
timeFail=datetime.utcnow()

print('humedad')
#if (timeFail - timedelta(minutes=5) < timeTTN):
if (timeFail - timedelta(minutes=0) < timeTTN):
    print(hum)
else:
    try:
        errorIndication, errorStatus, errorIndex, varBinds = next(
           getCmd(SnmpEngine(),
           CommunityData('greencore', mpModel=0),
           UdpTransportTarget(('10.42.22.182', 161)),
           ContextData(),
           ObjectType(ObjectIdentity('1.3.6.1.4.1.5.0')))
        )
        if errorIndication:
            print("Error: " + errorIndication)
        elif errorStatus:
            print('%s at %s' % (errorStatus.prettyPrint(),
                                errorIndex and varBinds[int(errorIndex) - 1][0] or '?'))
        else:
            snmpdata = str(varBinds[0]).split("= ")
            print(snmpdata[1])

    except:
        print("-1")
