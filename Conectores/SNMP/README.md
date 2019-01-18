# SNMP

## Descripción

El script de python ``db2snmp.py``, lee de una base de datos TinyDB la cual ha sido alimentada por ``ttn2db.py``, y exporta según el valor consultado en formato de SNMP. Este script está hecho para ser llamado por medio de ``snmpd``.

## Instrucciones de instalación

### venv
```
sudo python3 -m "venv" /opt/venv
sudo /opt/venv/bin/pip3 install -r requirements.txt
sudo /opt/venv/bin/python3 db2snmp.py
```

### snmpd

sudo apt install snmpd
sudo vim /etc/snmp/snmpd.conf

``` 
rocommunity strong-password

extend sensor01-humedad /opt/venv/bin/python3 /.../TTN_SNMP/Conectores/SNMP/db2snmp/sensor-01-humedad.py
```

Se debe definir una línea "extend ..." por cada sensor, para que envíe datos por SNMP, al ejecutar el script retonar un string con el nombre del sensor y el valor capturado.

Esto generará un OID especial: .1.3.6.1.4.1.8072.1.3.2.4.1.2.16.115.101.110.115.111.114.48.49.45.104.117.109.101.100.97.100


- Prefijo: .1.3.6.1.4.1.8072.1.3.2.4.1.2.
- El resto del OID se conforma por:
  1. Cantidad de caracteres de la descripción del extend, en el ejemplo "sensor01-humedad" = 16 caracteres
  2. La representación decimal ASCI de cada uno de los caracteres, en el ejemplo "sensor01-humedad":
       115 : ASCII decimal de 's'
       101 : ASCII decimal de 'e'
       110 : ASCII decimal de 'n'
       115 : ASCII decimal de 's'
       111 : ASCII decimal de 'o'
       114 : ASCII decimal de 'r'
       48  : ASCII decimal de '0'
       49  : ASCII decimal de '1'
       45  : ASCII decimal de '-'
       104 : ASCII decimal de 'h'
       117 : ASCII decimal de 'u'
       109 : ASCII decimal de 'm'
       101 : ASCII decimal de 'e'
       100 : ASCII decimal de 'd'
       97  : ASCII decimal de 'a'
       100 : ASCII decimal de 'd'


Por lo cual desde el servidor de monitoreo se puede consultar vía SNMP para confirmar que los datos se leen correctamente, para esto hay 2 formas

1. Consulta SNMP de todos los OID extendidos
``` 
snmpwalk -On  -v1 -c strong-password IP  NET-SNMP-EXTEND-MIB::nsExtendObjects
```

2. Consulta SNMP específica del OID de interés:

```
snmpwalk -On  -v1 -c strong-password IP .1.3.6.1.4.1.8072.1.3.2.4.1.2.16.115.101.110.115.111.114.48.49.45.104.117.109.101.100.97.100

.1.3.6.1.4.1.8072.1.3.2.4.1.2.16.115.101.110.115.111.114.48.49.45.104.117.109.101.100.97.100.1 = STRING: "humedad"
.1.3.6.1.4.1.8072.1.3.2.4.1.2.16.115.101.110.115.111.114.48.49.45.104.117.109.101.100.97.100.2 = STRING: "50"
```

