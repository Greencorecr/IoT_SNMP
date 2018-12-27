# Conector de TinyDB a SNMP

from datetime import datetime, timedelta
import time
#from time import strptime
from time import gmtime, strftime
from tinydb import TinyDB, Query
db = TinyDB('../MQTT/sensores.json')
Sensores = Query()

search=db.get(Sensores.dev_id == 'ttgo-mapper')

# Medimos el tiempo según TTN, el tiempo de hace 5 minutos. Si no hemos recibido datos, el sensor responde "0"
timeTTN = datetime.strptime((search.get("time")[0:10] + " " + search.get("time")[11:19]), "%Y-%m-%d %H:%M:%S")
timeFail=datetime.utcnow() 
print(timeFail - timedelta(minutes=5), timeTTN)
#print (timeFail - timedelta(minutes=5) > timeTTN)

print('.1.3.6.1.2.1.25.1.8.2')
print('gauge')
if (timeFail - timedelta(minutes=5) < timeTTN):
    print(search.get("temp"))
else:
    print("0")


