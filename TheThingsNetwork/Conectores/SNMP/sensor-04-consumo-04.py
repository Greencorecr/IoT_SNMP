# Conector de TinyDB a SNMP
# Consumo Electrico

from datetime import datetime, timedelta
from tinydb import TinyDB, Query
dbpath = "/opt/MQTT/BD/sensor-04-consumo-04.json"
db = TinyDB(dbpath)
Sensores = Query()

search=db.get(Sensores.dev_id == 'sensor-04-consumo')

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
#print (timeFail - timedelta(minutes=10), timeTTN)
    
print('amp4')

if (timeFail - timedelta(minutes=10) < timeTTN):
    print(search.get("amp4"))
else:
    # 2 - no se está recibiendo información reciente
    print("-1")
