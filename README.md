# TTN_SNMP
Dispositivos con The Things Network para conectar a SNMP

## [Conector con MQTT](https://github.com/Greencorecr/TTN_SNMP/tree/master/Conectores/MQTT)

Se mantiene un daemon escuchando en la plataforma de MQTT de TheThingsNetwork, escuchando mensajes de los diferentes sensores, y guardando en una base de datos de TinyDB en formato JSON, los valores decodificados que recibe, así como un timestamp del momento en el que recibe el mensaje.

## [Conector con SNMP](https://github.com/Greencorecr/TTN_SNMP/tree/master/Conectores/SNMP)

Dependiendo del OID consultado, snmpd llama al conector que saca de la base de el valor del sensor, así como verificando que el timestamp no sea más viejo que 5 minutos, lo que quiere decir que el sensor ha dejado de comunicarse, en el caso en que retorna "0" como valor del sensor. Se utiliza la técnica de este [ejemplo](https://github.com/fede2cr/raspberry-pi_snmp/blob/master/snmpd-example/snmpd.conf)

## Sensores

![Case con TTGO](https://github.com/Greencorecr/TTN_SNMP/tree/master/img/Case+TTGO.jpg)
![Case con PCB](https://github.com/Greencorecr/TTN_SNMP/tree/master/img/Case+PCB.jpg)

- [Sensor de Gotas](https://github.com/Greencorecr/TTN_SNMP/tree/master/Sensores/Gotas)
- [Sensor de Puerta](https://github.com/Greencorecr/TTN_SNMP/tree/master/Sensores/Puerta)
- [Sensor de caída eléctrica]()
- [Sensor de consumo eléctrico]()
- [Sensor de temperatura/humedad](https://github.com/Greencorecr/TTN_SNMP/tree/master/Sensores/TempHum)
