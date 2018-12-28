import unittest
 
class PruebaTiempo(unittest.TestCase):
    """
    
    """
 
    def prueba_resta_tiempo(self):
        """
        Revisa la formula para verificar si han pasado 5 minutos desde 
        la última actualización
        """
        timeMQTT="2018-12-27T23:35:00.000000000Z"
        timeTTN=timeMQTT[0:10] + " " + timeMQTT("time")[11:19])

        timeFail="2018-12-27 23:30:00.000000"
        result = timeFail - timedelta(minutes=5) = timeTTN
        self.assertEqual(result, true)
