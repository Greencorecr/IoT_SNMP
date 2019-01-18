# Check_MK

## Descripción

El script de python ``db2snmp.py``, lee de una base de datos TinyDB la cual ha sido alimentada por ``ttn2db.py``, y exporta según el valor consultado en formato de SNMP. Desde el servidor de monitoreo se debe configurar un chequeo para que consulte la información de los sensores vía SNMP, y la asocie con un servicio nuevo a monitorear.

## Instrucciones de configuración

### 1. Haber realizado la configuración del conector SNMP, y que la consulta por snmpwalk retorne datos reales.

TTN_SNMP/Conectores/SNMP/README.md


### 2. Crear un script de chequeo local:

```
sudo vim /opt/omd/sites/pruebas/local/share/check_mk/checks/sensor01_humedad
```

```
#!/usr/bin/python

def inventory_sensor01_humedad(info):
   inventory = []
   for line in info:
        tipo = line[0]
        valor = line[1]
        inventory.append((tipo, valor))
   print inventory
   return inventory

def check_sensor01_humedad(item, _no_params, info):
   for line in info:
        tipo = line[0]
        valor = line[1]
        if tipo == "humedad" and valor != "0" and valor != "12336":
           infotext = "humedad actual: " + valor
           return 0, infotext, [ ("humedad", valor) ]
        else:
           infotext = "humedad actual: " + valor
           return 2, infotext, [ ("humedad", valor) ]

check_info["sensor01_humedad"] = {
    "check_function"        : check_sensor01_humedad,
    "inventory_function"    : inventory_sensor01_humedad,
    "service_description"   : "Sensor",
    "snmp_info"             : ( ".1.3.6.1.4.1.8072.1.3.2.4.1.2.16.115.101.110.115.111.114.48.49.45.104.117.109.101.100.97.100", [ "1", "2" ] ) ,
    "has_perfdata"          : True,
}
```

### 3. Agregar el host a monitorear

Se debe agregar el host a monitorear (Raspberry Pi) con la correspondiente contraseña SNMP Community y sin monitoreo por agente, con el fin de que comience a escanear servicios a nivel de SNMP únicamente.

### 4. Validar chequeo local
Una vez agregado el host, ejecutar el siguiente comando para validar el chequeo local, para lo cual es necesario convertirse en el usuario propietario del sitio a nivel de OMD, en el siguiente ejemplo se asume que el sitio de Check_MK se llama "pruebas" y el host "raspberry"

``` 
sudo su - pruebas
```

```
OMD[pruebas]:~$ check_mk -v --checks sensor01_humedad -I raspberry

Discovering services on: raspberry
raspberry:
+ FETCHING DATA
 [snmp] Execute data source
 [piggyback] Execute data source
+ EXECUTING DISCOVERY PLUGINS (1)
[(u'humedad', u'52')]
SUCCESS - Found nothing new
```

Si el comando anterior funciona correctamente e imprime los datos recolectados, ejecutamos el siguiente comando para hacer un chequeo real:

```
OMD[pruebas]:~$ cmk --no-cache -nv raspberry
Check_MK version 1.5.0p9
+ FETCHING DATA
 [snmp] Execute data source
 [piggyback] Execute data source
CPU load             OK - 15 min load 0.08
CPU utilization      OK - 5.0% used
...
...
SNMP Info            OK - Linux raspberry 4.14.79-v7+ #1159 SMP Sun Nov 4 17:50:20 GMT 2018 armv7l, raspberry, Unknown, root
Sensor humedad       OK - humedad actual: 52
...

```

En este punto el chequeo local es funcional, por lo cual desde la interfaz web de Check_MK se puede observar como un nuevo servicio monitoreado.
