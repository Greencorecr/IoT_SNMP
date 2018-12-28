import unittest
from datetime import datetime, timedelta

class PruebaConector(unittest.TestCase):
    """
      Comprueba las base de datos y la manipulacion de fechas
    """
 
    def test_resta_tiempo(self):
        """
        Revisa la formula para verificar si han pasado 5 minutos desde 
        la última actualización
        """
        timeMQTT="2018-12-27T23:30:00.000000000Z"
        timeTTN=datetime.strptime(timeMQTT[0:10] + " " + timeMQTT[11:19], "%Y-%m-%d %H:%M:%S")

        timeFail=datetime.strptime("2018-12-27 23:35:00",  "%Y-%m-%d %H:%M:%S")
        result =  (timeFail - timedelta(minutes=5) == timeTTN)
        print (result)
        self.assertEqual(result, True)


if __name__ == '__main__':
    unittest.main()        
