# SNMP

## Descripción

El script de python ``db2snmp.py``, lee de una base de datos TinyDB la cual ha sido alimentada por ``ttn2db.py``, y exporta según el valor consultado en formato de SNMP. Este script está hecho para ser llamado por medio de ``snmpd``.

## Instrucciones de instalación

```
python3 -m "venv" venv
venv/bin/pip3 install -r requirements.txt
venv/bin/python3 db2snmp.py
```
