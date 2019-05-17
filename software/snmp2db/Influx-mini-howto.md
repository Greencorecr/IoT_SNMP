# InfluxDB Mini Howto

Para depurar y desarrollar con influxdb es importante conocer algunos comandos básicos en dos tipos de clientes.

## InfluxDB Client

```
sudo apt install influxdb-client
influx
```
```sql
> show databases;

> select last(*) from measures;
ERR: database name required


> use sensores;

> show measurements

> select Last(*) from sensorgotas

> drop measurement sensoramps

```

## Python

Puede crear un archivo con este código, o también escribir desde una terminal:

```
from influxdb import InfluxDBClient

# InfluxDB
influxHost='localhost'
influxPort=8086
influxUser = 'root'
influxPassword = 'root'
influxDbname = 'sensores'
influxDbuser = 'greencore'
influxClient = InfluxDBClient(influxHost, influxPort, influxUser, influxPassword, influxDbname)


json_body = [{'measurement': 'sensorXXX',...

influxClient.write_points(json_body)
```
