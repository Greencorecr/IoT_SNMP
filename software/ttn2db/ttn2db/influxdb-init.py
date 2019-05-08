from influxdb import InfluxDBClient

def main(host='localhost', port=8086):
    user = 'root'
    password = 'root'
    dbname = 'sensores'

    client = InfluxDBClient(host, port, user, password, dbname)

    print("Create database: " + dbname)
    client.create_database(dbname)

    print("Create a retention policy")
    client.create_retention_policy('awesome_policy', '3d', 3, default=True)

if __name__ == '__main__':
    main()
