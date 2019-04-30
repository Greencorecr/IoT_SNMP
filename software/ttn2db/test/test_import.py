import unittest


class TestTTN2DB(unittest.TestCase):
    def test_import(self):
        import time
        import ttn
        from influxdb import InfluxDBClient

if __name__ == '__main__':
    unittest.main()
