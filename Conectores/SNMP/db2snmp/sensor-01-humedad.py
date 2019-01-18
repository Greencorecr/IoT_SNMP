# Conector de TinyDB a SNMP
# Temperatura y Humedad

from datetime import datetime, timedelta
from tinydb import TinyDB, Query
db = TinyDB('/opt/UNA/MQTT/sensores.json')
Sensores = Query()

search=db.get(Sensores.dev_id == 'sensor-demo-humedad')

def TTN2time(search):
    """
    Convierte el formato de hora de TTN, a algo usable con datetime
    """
    timeTTN = datetime.strptime((search.get("time")[0:10] + " " + search.get("time")[11:19]), "%Y-%m-%d %H:%M:%S")
    return timeTTN

# Medimos el tiempo según TTN, el tiempo de hace 5 minutos. Si no hemos recibido datos, el sensor responde "0"
timeTTN=TTN2time(search)
timeFail=datetime.utcnow()
# Debug
#print (timeFail - timedelta(minutes=5), timeTTN)

print('humedad')
if (timeFail - timedelta(minutes=5) < timeTTN):
    print(search.get("hum"))
else:
    print("0")
