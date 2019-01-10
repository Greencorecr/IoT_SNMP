# MQTT

## Descripción

El script de python ``ttn2db.py`` se encarga de leer desde TTN paquetes de MQTT, e insertarlos en una base de datos de TinyDB. Se recomienda ejecutar en conjunto con SystemD como un servicio, y es capaz de escuchar varios sensores de forma simultánea, agregando sus datos en la base de datos según el tipo de sensor.


## Instrucciones de instalación

```
python3 -m "venv" venv
venv/bin/pip3 install -r requirements.txt
venv/bin/python3 ttn2db.py
```
