import unittest
from datetime import datetime, timedelta
from tinydb import TinyDB, Query
from db2snmp.db2snmp import TTN2time



class PruebaConector(unittest.TestCase):
    """
    Comprueba las base de datos y la manipulacion de fechas
    """
 
    def testRestaTiempo(self):
        """
        Revisa la formula para verificar si han pasado 5 minutos desde 
        la última actualización
        """
        db = TinyDB('../../MQTT/test/sensores.json')
        Sensores = Query()
        search=db.get(Sensores.dev_id == 'sensor-demo')
        db.close()
        timeTTN = TTN2time(search)
        timeFail=datetime.strptime("2018-12-27 23:35:00",  "%Y-%m-%d %H:%M:%S")
        result =  (timeFail - timedelta(minutes=5) == timeTTN)
        self.assertTrue(result)


if __name__ == '__main__':
    unittest.main()        
