import unittest


class TestTTN2DB(unittest.TestCase):
    def test_import(self):
        import time
        from influxdb import InfluxDBClient
        import pysnmp

if __name__ == '__main__':
    unittest.main()
